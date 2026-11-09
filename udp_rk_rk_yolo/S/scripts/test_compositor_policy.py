"""Policy tests use a fake command runner; never change this PC's desktop."""
import subprocess
from compositor_policy import prepare_compositor


def case(replies, expected, *, env=None, available=True):
    calls = []
    def run(command, **kwargs):
        calls.append(command)
        assert kwargs['timeout'] == 5
        return subprocess.CompletedProcess(command, *replies.pop(0))
    actual = prepare_compositor({'DISPLAY': ':0'} if env is None else env,
                                run, lambda _: 'xfconf-query' if available else None)
    assert actual is expected and not replies
    return calls


assert not case([], False, env={'DISPLAY': ':0', 'VISION_KEEP_COMPOSITOR': '1'})
assert not case([], False, env={})
assert not case([], False, available=False)
assert len(case([(0, 'false\n', '')], True)) == 1
calls = case([(0, 'true\n', ''), (0, '', ''), (0, 'false\n', '')], True)
assert calls[1][-2:] == ['--set', 'false']
calls = case([(1, '', 'Property does not exist'), (0, '', ''), (0, 'false\n', '')], True)
assert '--create' in calls[1]
assert len(case([(1, '', 'Cannot connect to D-Bus')], False)) == 1
case([(0, 'true\n', ''), (1, '', 'failed'), (0, 'true\n', '')], False)
print('COMPOSITOR POLICY PASS: idempotent, opt-out, unavailable session, verified change, no real desktop writes')
