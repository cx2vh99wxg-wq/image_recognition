# S 转向测角闭环

保留 `steering_init/set_target_deg/get_state/cancel`，新增 `steering_bind`、
`steering_confirm_reference`、`steering_tick`。API单线程使用，持续tick；设置目标不表示已经到位。

状态：未确认零点→等待稳定→短脉冲→释放/等待稳定→再规划→到位。
每目标最多6次脉冲、最多一次反向；未完成目标更新不重置预算。
分辨率不足返回 `STEERING_RESOLUTION_LIMIT`，不把0响应/大跳变平均成精确小角度。
过期、跨会话、反向异常、超时、STOP、人工接管或写失败均撤销输出。
S/R可清除控制故障，不能授予新M会话的物理零点。

## 自动标定

固定车身、停止vehicle_control，确认SPI无人占用，创建 `/tmp/steering-bench.active`。
已有 `steering_bench.status()` 仍逐脉冲检查。视频8888和IMU54321相互独立；
不能两个消费者抢占54321。

```sh
cd /home/linaro/S/steering
python3 tools/build.py --test
python3 tools/build.py --hardware
python3 tools/build.py --monitor
python3 tools/calibration/steering_reference.py --expected-session SESSION --confirm-physical-center --fixed-chassis --output logs/center.reference
python3 tools/calibration/steering_auto_calibrate.py --run-id 202610310001 --output logs/batch_202610310001 --fixed-chassis --confirm-zero --reference-session SESSION --zero-raw-deg RAW_ZERO
```

默认双向5/10/15/17.5/18/20/25/30/40/50/60ms，每项两次。每次重新取得稳定基线，
核对会话；需要时按已测模型预置安全起点。未知幅度边界过大或无法安全预置时记录
`PARTIAL_COVERAGE`，不强行测试。失败尝试释放双方，不自动重放失败试次。
记录方向有效响应不足、未回中、采样/时序失败，不能冒充完整标定。

A试次、总批次和 `STEER_BENCH_RESPONSE_V1` JSON持续落盘；保留0响应、分布、
起始角覆盖、优先级、软件实测脉宽。`response_model.table`是同一JSON的校验导出，
不另拟合、不插值外推。模拟数据禁止用于真实运行导出。
Windows支持 `--simulate`；硬件仅Linux。独立T目标试验不混入A标定。
同一物理零点和M会话内，可用 `--continue-model PREVIOUS_JSON` 从非零位置
补测缺项；先复算并校验所有A样本SHA和参考绑定，不能跨新session续接。
发现机械障碍或安装改变时，应将旧数据标记invalidated并隔离运行表，
重新确认零点后建全新批次。被撤销的数据不能拟合、导出或续测。

## 台架目标及统一IO

```sh
./actuator/build/steering_goal_fspi --backend fspi --fixed-chassis --model logs/batch_202610310001/response_model.table --reference logs/center.reference --target-deg 5
# 完成台架、释放并退出脉冲工具后，才移除bench标记并启动整车：
STEERING_CONTROL_MODE=closed-loop \
STEERING_RESPONSE_TABLE=/home/linaro/S/steering/logs/batch_202610310001/response_model.table \
STEERING_REFERENCE_FILE=/home/linaro/S/steering/logs/center.reference \
STEERING_CURVE_GAIN=0.25 STEERING_LANE_TARGET_LIMIT_DEG=12 \
/home/linaro/S/fspi/build/vehicle_control
```

主进程内接收角度并调用同一控制器；工具与主进程共用 `/tmp/steering-actuator.lock`。
`STEER_PULSE_BEGIN/END/RESPONSE`分别记录请求、软件实际时长和稳定后转角；
`GOAL_RESULT state_name=RESOLUTION_LIMIT`表示未到位，不是成功或仍缺实现。
闭环模式不调用原0.5/0.6/1.5秒固定脉冲，模型/零点/接收器失败也不回退长脉冲。
未选择闭环时保留原模式。闭环tick约0.5ms，原策略约100ms；活动脉冲避开慢扫描。
启动方向单次事务，释放始终尝试左右。每5ms核对STOP、复位/FSPI许可与HMI。
GO/BACK、档位、行人刹停、红灯GO和实线HMI拦截沿用原协议。

视觉角不是机械角，按可配置增益映射并限幅；置信度<60、S弯/异常、原NN结果
超过500ms不再控制。`STEERING_PRIORITY`须匹配模型cell优先级，默认0，非零需root/FIFO。

## 保活和验收边界

原reg5心跳保留，新增1秒只读状态/日志心跳，脉冲/台架时避让、不补发突发。
不周期按转向或STOP键。M静止时仍100Hz采样发包。该心跳证明链路活性，
不能证明会复位射频遥控器/车身的省电计时器；目前无已确认的无动作唤醒协议。

V1/V2请求≤60ms、预测包络±18°，给机械±20°留余量。Linux不是硬实时，软件脉宽异常
会停止并排除样本。SIGTERM/普通故障处理释放；SIGKILL、系统崩溃或SPI完全失联
的硬截止需要FPGA独立看门狗，不能由用户态软件保证。
单MPU不能分离车身偏航与方向盘相对车身转角；本轮固定车身闭环验证不等于
行驶精度、安装轴、绝对网络延迟或机械惯性已经验收。

## 连续大角度候选模式（V3）

大角度不要求拆成很多短脉冲。V3在误差≥6°、起点在中位附近或动作朝向中位时，
先执行一次连续低电平，使用实时角度、角速度和本次固定安装采集的释放余转提前停止。
到位后按±3°验收；超过容差最多补两次≤60ms实测表内动作，仍不到位报告
`RESOLUTION_LIMIT`。不反复抖动、不自动放宽容差。靠近端点向外的小动作仍使用原表。

连续动作需要显式V3表，原V1/V2策略保持原有上限。V3头格式为：

```text
STEER_RESPONSE_TABLE_V3 <原V2单脉冲cell数量>
BURST <上限us> <M会话> <物理零点raw> <左估计ms> <左保护ms> <右估计ms> <右保护ms> <左估计余转上限deg> <左保护余转上限deg> <右估计余转上限deg> <右保护余转上限deg>
<原V2各cell>
```

上限必须为61～250ms整毫秒，FIFO优先级必须10，目标仅±15°以内。每个目标保留
600ms累计请求预算，执行器超时、反馈过期、会话改变、STOP及人工接管均释放。
保护预测采用独立的峰值余转边界，不能把峰值裁成平均余转；用S端接收时间辨识，
不假定两板时钟已同步。左右参数独立。当前角度与速度均来自真实M遥测。

`burst_response.py`保存连续动作的真实脉宽、起点、终点、峰值和释放后轨迹。
`export_runtime.py --burst-profile PROFILE.json`核对所有源样本SHA、同一物理参考、
两方向覆盖、实际用过的参数及峰值保护余量后，合并V2细调表生成V3候选表。
导出和软件测试不代表已通过实车验收；需再运行`steering_validate_native.py`，
用独立目标验证±12°/±15°与回中、释放后的稳定角度。验证轨迹不回灌训练数据。

生产启用仍使用上述`STEERING_CONTROL_MODE=closed-loop`入口，表路径指向验收合格的V3表，
并设置`STEERING_PRIORITY=10`。新M会话或重新安装后旧参考失效，不能静默沿用。
该模式只改变S端软件转向策略，不改变HMI码、FPGA引脚或M端车道识别。

`S/Makefile`的FSPI启动入口通过`tools/launch_vehicle_control.py`执行原二进制，PID不变。
没有`S/steering/runtime.json`时保留原环境；显式安装验收合格的
`STEER_RUNTIME_LAUNCH_V1`配置后，正常`make run`/`make run-fspi`即可选择同一闭环表。
配置不存在/禁用与配置损坏不同：损坏配置拒绝启动，不静默回退。闭环独占IMU接收
端口，不要另开被动角度接收器抢占54321。M新会话仍需重新确认物理零点。

真实推理约3～4Hz，500ms有效期从输入拍摄时刻开始计算；结果抵达S时可能只剩
80～200ms。空闲时持续积累真实IMU静止历史，新目标复用同会话且连续新鲜的历史，
避免每次重新等300ms而被下一次过期取消。实际转动、断流或中断活动脉冲会撤销
该历史，动作之后仍须完整稳定确认。未延长视觉TTL，也未降低置信度阈值。

道路输入位置以`S/include/camera_views.h`为准：当前弯道分析读取M原始四格左上
320×240区域；该区域复制前中摄像头（原始右下格），在S六路总览中是上排中间
`Front Center`、HMI `m3`。六路总览上排左侧为`Front Left/m4`，不是同一ROI。
验证时先抓取`/tmp/m_lane_snapshot.ppm`核对实际模型输入，不能仅凭总览格号判断。

2026-10-31实机曾出现30ms请求实际33.54ms的时序保护退出。释放前最后6ms不再
启动新的SPI状态读取，而复用最近且年龄小于12ms的逻辑快照；其余期间仍按5ms刷新。
执行器截止时间、100ms角度新鲜度和原来的脉宽容差保持不变，FPGA的STOP/人工优先
仍直接作用于引脚。带7ms模拟SPI阻塞的回归复现旧版故障并验证新版释放；Linux调度
或SPI写本身的长阻塞仍可能触发保护，不能把软件回归视为硬实时保证。

### 上电后弯道有显示但完全没有转向

先核对M测角会话与`STEERING_REFERENCE_FILE`，不要先放大脉冲。M板/IMU服务
重新启动会改变会话，旧机械零点不再适用。未绑定时车控终端每5秒输出
`[STEER_WAIT_REFERENCE]`，包括`expected_session`、`received_session`和等待原因；
反馈或会话状态变化也会立即输出。此时自动转向仍被禁止，不自动采用开机0度。
只有现场确认机构已在机械直行中位、安装未改变后，才能以新鲜静止遥测重新绑定。

弯道映射增益与到位容差是两项独立设置。原25度道路估值乘0.25得到6.25度目标，
在3度容差下可能只转动约3.3度就满足到位条件；这与没有输出脉冲不同。
调整增益后必须独立验证两方向目标和回中，不靠延长视觉有效期或绕过零点保护。
