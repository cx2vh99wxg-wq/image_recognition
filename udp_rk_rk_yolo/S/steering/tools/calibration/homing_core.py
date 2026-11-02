"""Bounded endpoint-midpoint homing, independent of a boot-relative IMU zero.

The endpoint angle origin is arbitrary. An endpoint is only a candidate until
the complete, opposite-direction stroke agrees with the verified mechanism.
Hardware IO must enforce source freshness, session and contact arbitration.
"""
from dataclasses import dataclass
import math
import statistics


class HomeError(RuntimeError):pass


@dataclass
class HomeLimits:
    pulse_us:int=30000
    min_span_deg:float=36
    max_span_deg:float=44
    stall_deg:float=.15
    stall_count:int=3
    endpoint_rebound_deg:float=1.5
    full_stroke_rebound_deg:float=3
    max_seek_pulses:int=24
    max_total_us:int=1500000
    fine_reserve_us:int=360000
    max_relative_deg:float=50
    inner_deg:float=14
    tolerance_deg:float=3

    def validate(self):
        if not 1000<=self.pulse_us<=30000 or not 20<=self.min_span_deg<self.max_span_deg<=50:
            raise HomeError('Invalid qualified homing limits')
        if not 0<self.stall_deg<=.2 or self.stall_count!=3 or not 2<=self.max_seek_pulses<=24 or not 0<=self.endpoint_rebound_deg<=1.5:
            raise HomeError('Invalid endpoint confirmation limits')
        if not self.endpoint_rebound_deg<=self.full_stroke_rebound_deg<=3:
            raise HomeError('Invalid verified-stroke rebound limits')
        if not 0<self.fine_reserve_us<self.max_total_us<=1500000 or not self.max_span_deg<=self.max_relative_deg<=50:
            raise HomeError('Invalid homing travel/request budget')
        if not 0<self.tolerance_deg<=3 or not 6<=self.inner_deg<=14:
            raise HomeError('Invalid homing center limits')


def run_homing(io,limits=None):
    """IO: stable(), pulse(direction,us), center(mid,tol,budget), release().

    stable returns real raw-relative degrees, not a declared physical zero.
    center returns the measured raw-relative angle plus its actual requests.
    """
    c=limits or HomeLimits();c.validate()
    try:
        start=io.stable()
        if not math.isfinite(start):raise HomeError('Invalid initial real angle')
    except BaseException:
        io.release()
        raise
    current=start;used=0;records=[]
    def observed():
        value=io.stable()
        if not math.isfinite(value) or abs(value-start)>c.max_relative_deg:
            raise HomeError('Homing relative travel exceeded its bounded envelope')
        return value
    def pulse(direction,phase,rebound_floor=None,verified_rebound=None):
        nonlocal used,current
        if used+c.pulse_us>c.max_total_us-c.fine_reserve_us:
            raise HomeError('Homing request budget exhausted')
        before=current
        io.pulse(direction,c.pulse_us);used+=c.pulse_us
        after=observed();delta=after-before
        accepted_rebound=verified_rebound is not None and verified_rebound(after)
        if delta*direction<-.4 and not (rebound_floor is not None and
            abs(delta)<=c.endpoint_rebound_deg and direction*after>=rebound_floor) and not accepted_rebound:
            raise HomeError('Measured rotation opposes homing contact')
        records.append(dict(phase=phase,direction=direction,requested_us=c.pulse_us,
                            before_raw_deg=before,after_raw_deg=after,delta_deg=delta,
                            verified_full_stroke_rebound=accepted_rebound))
        current=after
        return delta
    def seek(direction,phase,opposite_endpoint=None):
        positions=[];best=direction*current
        def qualified_window(value=None):
            recent=(positions if value is None else positions+[value])[-c.stall_count:]
            return (opposite_endpoint is not None and len(recent)==c.stall_count
                and all((v-opposite_endpoint)*direction>=0 for v in recent)
                and c.min_span_deg<=max(abs(v-opposite_endpoint) for v in recent)<=c.max_span_deg
                and c.min_span_deg<=abs(statistics.median(recent)-opposite_endpoint)<=c.max_span_deg
                and max(recent)-min(recent)<=c.full_stroke_rebound_deg
                and direction*(recent[-1]-recent[0])<=c.stall_deg)
        for _ in range(c.max_seek_pulses):
            # 反向累计行程同样受限，开机即顶在止挡的情形也包括在内；
            # 不能反复朝相反方向小步移动，
            # 仅靠多次局部小响应就认定已经构成一个完整端点。
            pulse(direction,phase,rebound_floor=best-c.endpoint_rebound_deg,verified_rebound=qualified_window)
            best=max(best,direction*current);positions.append(current)
            recent=positions[-c.stall_count:]
            # 已验证的完整行程可将弹性止挡回弹
            # 与方向错误区别开。判到止挡后立刻结束寻边，
            # 不为得到近似重复样本继续撞击止挡。
            if qualified_window():return statistics.median(recent)
            if (len(recent)==c.stall_count and max(recent)-min(recent)<=max(c.stall_deg,c.endpoint_rebound_deg)
                and direction*(recent[-1]-recent[0])<=c.stall_deg):
                return sum(recent)/len(recent)
        raise HomeError('Endpoint not observed before phase pulse limit')
    try:
        left=seek(1,'seek_left')
        right=seek(-1,'seek_right',opposite_endpoint=left)
        span=left-right
        if abs(span)<1:
            raise HomeError('No credible steering movement after both contacts; check motor/remote power, pairing, mode or obstruction; zero remains unconfirmed')
        if not c.min_span_deg<=span<=c.max_span_deg:
            raise HomeError('Endpoint stroke %.3f outside qualified %.3f..%.3f degrees'%(span,c.min_span_deg,c.max_span_deg))
        midpoint=(left+right)/2
        # 普通标定控制仍限于 +/-18deg；只有归中的
        # 向内粗调允许在机构尚未进入该区间前使用。
        for _ in range(6):
            if abs(current-midpoint)<=c.inner_deg:break
            before_error=abs(current-midpoint)
            pulse(1 if current<midpoint else -1,'inward_to_calibrated_region')
            if abs(current-midpoint)>=before_error-.1:
                raise HomeError('No inward progress toward endpoint midpoint')
        if abs(current-midpoint)>c.inner_deg:
            raise HomeError('Could not enter calibrated inner region')
        final,requests=io.center(midpoint,c.tolerance_deg,c.max_total_us-used)
        if any(not 0<n<=60000 for n in requests) or used+sum(requests)>c.max_total_us:
            raise HomeError('Fine centering exceeded request budget')
        used+=sum(requests)
        measured=observed()
        if not math.isfinite(final) or abs(final-midpoint)>c.tolerance_deg or abs(measured-midpoint)>c.tolerance_deg:
            raise HomeError('Real feedback did not settle at endpoint midpoint')
        return dict(schema='STEER_ENDPOINT_HOME_V1',start_raw_deg=start,left_raw_deg=left,
                    right_raw_deg=right,span_deg=span,midpoint_raw_deg=midpoint,
                    final_raw_deg=measured,error_deg=measured-midpoint,
                    pulse_records=records,fine_requests_us=requests,total_requested_us=used,
                    center_source='verified_mechanical_endpoint_midpoint',passed=True)
    finally:io.release()
