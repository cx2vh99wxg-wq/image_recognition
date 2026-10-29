# 共同接口 v1

本目录是 M/S 唯一的数据与线协议定义，两端通过各自构建入口引用公共头，不复制另一套。
公共目录不属于 M 或 S，也不编译任何传感器/执行器实现。
分支后变更接口应由双方协调后交一人提交。

## 语义

- 角度：机械中位为 0，左正右负，主机算法使用 degree；线协议使用有符号 millidegree。
- 角速度：相同转轴和正负号，degree/second 或 millidegree/second。
- 原始加速度：m/s²，含重力；原始陀螺仪：degree/second，不能当作角度。
- RELEASE：释放左右控制，不表示物理居中。回中将由阶段 3 设置目标 0°实现。
- VALID：测量/估计可用；ZERO_VALID：已经建立并保持机械中位基准；SIMULATED：模拟数据。
- 终端相对积分从启动时记 0°，**不自动设置 ZERO_VALID**。
- 程序重启、重新归零或参考基准变化时，生产者换一个非零 session_id。

## STF1 反馈帧

固定 48 字节，所有多字节整数用大端序。禁止直接发送 C 结构体。

| 偏移 | 长度 | 内容 |
|---|---|---|
| 0 | 4 | ASCII `STF1` |
| 4 | 2 | 版本，1 |
| 6 | 2 | 长度，48 |
| 8 | 4 | session_id，非零 |
| 12 | 4 | sequence，新样本才递增，可按 uint32 回绕 |
| 16 | 4 | flags：bit0 VALID、bit1 ZERO_VALID、bit2 SIMULATED，其余必须 0 |
| 20 | 4 | angle_mdeg，有符号 int32 |
| 24 | 4 | rate_mdps，有符号 int32 |
| 28 | 4 | source_age_ms，发送时样本年龄，重复发送不得刷新采样时刻 |
| 32 | 8 | sample_time_ns，M 端源单调时刻 |
| 40 | 4 | 保留，必须 0 |
| 44 | 4 | 前 44 字节的 IEEE CRC32，大端；多项式 0xEDB88320，初值/最终异或均 0xFFFFFFFF |

解码函数只验证格式、CRC 和标志合法性，失败不修改输出。
解码成功不等于可驱动电机：VALID、ZERO_VALID、数据年龄、会话、序号和模拟标记仍需接收层判断。

## 阶段 3 接收层必须完成的工作（尚未实现）

1. 独立于视频帧调度收发；不占用原 `curve_angle` 字段。
2. 接收端记录本机单调时刻；不能直接相减 M、S 的单调时钟。
3. source_age 不包含未测量的网络传输时间；需要测量/约束端到端延迟，不能只凭刚收到包就认定样本新鲜。
4. 去重、拒绝乱序和旧会话；重复包不延长同一样本有效期。
5. 只有 VALID + ZERO_VALID 且新鲜、非模拟的反馈才允许实车闭环。
6. 反馈丢失、时间间隔异常、生产者重启时撤销自动输出并报告故障，不伪造 0°。

## API 状态

`S/steering/controller/steering_api.h` 定义阶段 3 的初始化、绝对目标角、状态查询和取消，属于 S 端控制 API。
当前 `S/steering/controller/steering_controller.c` 为显式 NOT_READY 实现，不会发出动作。
查询未知角度使用 NAN，feedback_valid=0，不能显示成已回中或已到位。
