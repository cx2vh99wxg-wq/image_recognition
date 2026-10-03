# UDP弯道信息控制小车运动技术原理

## 完整控制链路

```
┌─────────────────────────────────────────────────────────────────────┐
│                         M端（发送端）                                  │
│  - 检测弯道类型（左弯/右弯/直道/S弯）                                  │
│  - 计算置信度和角度                                                   │
└────────────────────────┬────────────────────────────────────────────┘
                         │ UDP网络传输
                         ↓ FrameHeader
┌─────────────────────────────────────────────────────────────────────┐
│                   S端UDP接收程序 (udp/csrc/main.cpp)                 │
│  第365行：recvfrom() 接收帧头                                         │
│  第430-435行：curve_detection_update() 写入共享内存                   │
└────────────────────────┬────────────────────────────────────────────┘
                         │ 共享内存
                         ↓ CurveDetectionSharedMemory (Key: 0x1234567E)
┌─────────────────────────────────────────────────────────────────────┐
│                   FSPI控制程序 (fspi/csrc/main.c)                     │
│  第511行：curve_detection_read() 读取共享内存                         │
│  第540-643行：根据弯道类型调用 vehicle_control()                      │
└────────────────────────┬────────────────────────────────────────────┘
                         │ FSPI接口
                         ↓ SPI总线写入
┌─────────────────────────────────────────────────────────────────────┐
│                         FPGA/硬件控制                                 │
│  寄存器地址2：左转控制                                                 │
│  寄存器地址3：右转控制                                                 │
│  值=0：启动运动   值=1：停止运动                                       │
└────────────────────────┬────────────────────────────────────────────┘
                         │ 电机驱动
                         ↓
                    小车实际转向
```

---

## 详细实现分析

### 第1步：M端UDP帧头结构 (udp/csrc/main.cpp 第74-87行)

```c
typedef struct {
    uint32_t magic;              // 魔数: FRAME_HEADER_MAGIC
    uint32_t frame_id;           // 帧ID
    uint32_t width;              // 图像宽度
    uint32_t height;             // 图像高度
    uint32_t data_size;          // 帧数据大小
    uint32_t line_count;         // 行数
    uint32_t checksum;           // 帧校验和
    uint32_t timestamp;          // 时间戳
    uint32_t curve_type;         // ★ 弯道类型: 0=无, 1=左弯, 2=右弯, 3=左右弯
    uint32_t curve_confidence;   // ★ 弯道检测置信度 (0-100)
    uint32_t curve_angle;        // ★ 弯道角度 (度数)
    uint32_t reserved;           // 预留字段
} FrameHeader;
```

**关键字段：**
- `curve_type`：决定车辆转向方向
- `curve_confidence`：用于判断是否执行转向（阈值60%）
- `curve_angle`：弯道角度信息（用于日志）

---

### 第2步：S端UDP接收并写入共享内存 (udp/csrc/main.cpp)

#### 接收帧头 (第365-382行)
```c
// 通过UDP接收帧头
received = recvfrom(receiver->socket_fd, &frame_header, sizeof(FrameHeader), 0,
                   (struct sockaddr*)&receiver->client_addr, &receiver->client_len);

// 验证帧头
if (frame_header.magic != FRAME_HEADER_MAGIC) {
    return 0;  // 魔数错误，继续下一帧
}
```

#### 写入共享内存 (第429-435行)
```c
// 将弯道信息写入共享内存，供FSPI读取
if (curve_detection_update(frame_header.frame_id,
                           (CurveType)frame_header.curve_type,
                           frame_header.curve_confidence,
                           frame_header.curve_angle) != 0) {
    fprintf(stderr, "警告: 弯道信息写入共享内存失败\n");
}
```

**curve_detection_update 函数实现** (S/csrc/shared_memory.c 第762-795行)：
```c
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle) {
    if (g_curve_shm_ptr == NULL) {
        return -1;
    }

    // 更新弯道检测结果到共享内存
    g_curve_shm_ptr->frame_id = frame_id;
    g_curve_shm_ptr->curve_type = curve_type;      // ★ 写入弯道类型
    g_curve_shm_ptr->confidence = confidence;      // ★ 写入置信度
    g_curve_shm_ptr->curve_angle = curve_angle;    // ★ 写入角度
    g_curve_shm_ptr->timestamp = (uint64_t)time(NULL) * 1000000;

    return 0;
}
```

---

### 第3步：FSPI读取共享内存 (fspi/csrc/main.c 第511行)

```c
// 读取弯道检测结果
if (curve_detection_read(&curve_frame_id, &curve_type,
                         &curve_confidence, &curve_angle) == 0) {
    // 成功读取，继续处理
}
```

**curve_detection_read 函数实现** (S/csrc/shared_memory.c 第797-816行)：
```c
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle) {
    if (g_curve_shm_ptr == NULL) {
        return -1;
    }

    // 从共享内存读取弯道检测结果
    *frame_id = g_curve_shm_ptr->frame_id;
    *curve_type = (CurveType)g_curve_shm_ptr->curve_type;    // ★ 读取弯道类型
    *confidence = g_curve_shm_ptr->confidence;                // ★ 读取置信度
    *curve_angle = g_curve_shm_ptr->curve_angle;              // ★ 读取角度

    return 0;
}
```

---

### 第4步：根据弯道类型控制车辆 (fspi/csrc/main.c 第540-643行)

#### 直道控制 (第540-557行)
```c
case CURVE_NONE:
    // 停止所有转向
    if (left_turn_active) {
        vehicle_control(2, 0);  // ★ 停止左转（地址2，值0）
        left_turn_active = 0;
    }
    if (right_turn_active) {
        vehicle_control(3, 0);  // ★ 停止右转（地址3，值0）
        right_turn_active = 0;
    }
    break;
```

#### 左弯控制 (第559-591行)
```c
case CURVE_LEFT:
    // 只有在置信度足够高时才执行转向
    if (curve_confidence >= 60) {
        // 停止右转（如果有的话）
        if (right_turn_active) {
            vehicle_control(3, 0);  // ★ 停止右转
            right_turn_active = 0;
        }

        // 启动左转
        if (!left_turn_active) {
            if (vehicle_control(2, 1) == 0) {  // ★ 启动左转（地址2，值1）
                left_turn_active = 1;
                left_turn_start_time = get_current_time();
            }
        }
    }
    break;
```

#### 右弯控制 (第593-622行)
```c
case CURVE_RIGHT:
    // 只有在置信度足够高时才执行转向
    if (curve_confidence >= 60) {
        // 停止左转（如果有的话）
        if (left_turn_active) {
            vehicle_control(2, 0);  // ★ 停止左转
            left_turn_active = 0;
        }

        // 启动右转
        if (!right_turn_active) {
            if (vehicle_control(3, 1) == 0) {  // ★ 启动右转（地址3，值1）
                right_turn_active = 1;
                right_turn_start_time = get_current_time();
            }
        }
    }
    break;
```

---

### 第5步：vehicle_control函数实现 (fspi/csrc/fspi_module.c 第475-515行)

```c
int vehicle_control(int command, int enable)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    // 参数检查
    if (command < 0 || command > 3) {
        return -1;  // 0=前进, 1=后退, 2=左转, 3=右转
    }

    if (enable != 0 && enable != 1) {
        return -1;  // enable必须是0或1
    }

    uint16_t addr = (uint16_t)command;              // ★ 寄存器地址（0-3）
    uint8_t control_value = (uint8_t)(enable ? 0 : 1);  // ★ 控制值（逻辑反转）

    // 注意：硬件逻辑反转
    // enable=1（启动运动）→ 写入0到硬件
    // enable=0（停止运动）→ 写入1到硬件

    printf("调试信息: 命令=%d, enable=%d, 地址=0x%04X, 发送值=%d\n",
           command, enable, addr, control_value);

    // 通过FSPI写入硬件寄存器
    int write_ret = fspi_write_data(addr, control_value);

    return write_ret;
}
```

**重要：硬件逻辑反转**
- `enable=1` (启动运动) → 向硬件写入 `0`
- `enable=0` (停止运动) → 向硬件写入 `1`

---

### 第6步：FSPI底层写入 (fspi/csrc/fspi_module.c)

#### fspi_write_data (第441-448行)
```c
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}
```

#### spi_write_reliable (第262-289行)
```c
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data)
{
    int retry_count = 0;
    int ret;

    // 重试机制，最多尝试WRITE_RETRY_COUNT次
    for (retry_count = 0; retry_count < WRITE_RETRY_COUNT; retry_count++)
    {
        ret = spi_write_3bytes(fd, addr, data);  // ★ 实际的SPI写入

        if (ret == 0) {
            return 0;  // 写入成功
        }

        // 重试前等待
        if (retry_count < WRITE_RETRY_COUNT - 1) {
            usleep(WRITE_RETRY_DELAY_US);
        }
    }

    return -1;  // 所有重试都失败
}
```

---

## 硬件寄存器映射

| 寄存器地址 | 功能 | 值=0 | 值=1 |
|-----------|------|------|------|
| 0 | 前进控制 | 前进运动 | 停止前进 |
| 1 | 后退控制 | 后退运动 | 停止后退 |
| 2 | **左转控制** | **左转运动** | **停止左转** |
| 3 | **右转控制** | **右转运动** | **停止右转** |
| 4 | 紧急制动 | 制动激活 | 正常运行 |

**注意：** 硬件逻辑是低电平有效（0=启动，1=停止）

---

## 控制决策表

| UDP弯道类型 | 置信度要求 | FSPI动作 | 硬件寄存器操作 |
|------------|----------|---------|---------------|
| CURVE_NONE (0) | 无要求 | 停止所有转向 | 写1到地址2和3 |
| CURVE_LEFT (1) | ≥60% | 启动左转，停止右转 | 写0到地址2，写1到地址3 |
| CURVE_RIGHT (2) | ≥60% | 启动右转，停止左转 | 写0到地址3，写1到地址2 |
| CURVE_BOTH (3) | 无要求 | 保持当前状态 | 不改变 |

---

## 安全保护机制

### 1. 置信度阈值保护
```c
if (curve_confidence >= 60) {
    // 只有置信度≥60%才执行转向
}
```

**目的：** 避免误判导致错误转向

### 2. 互斥转向保护
```c
// 左转时自动停止右转
if (curve_type == CURVE_LEFT && right_turn_active) {
    vehicle_control(3, 0);  // 先停止右转
    right_turn_active = 0;
}
```

**目的：** 防止同时左转和右转

### 3. 转向超时保护
```c
// 每0.5秒自动重置转向
if (left_turn_active && (current_time - left_turn_start_time) >= 0.5) {
    vehicle_control(2, 0);  // 停止
    usleep(50000);          // 延时50ms
    vehicle_control(2, 1);  // 重启
    left_turn_start_time = get_current_time();
}
```

**目的：** 防止转向卡死

### 4. 状态防抖动
```c
// 只在弯道类型变化或置信度显著变化时处理
int should_update = (curve_type != last_curve_type) ||
                   (abs((int)curve_confidence - (int)last_curve_confidence) > 20);
```

**目的：** 避免频繁切换转向状态

---

## 时序图

```
时间 →

M端:     [检测弯道]────→[发送UDP帧头]
              ↓              ↓
S端UDP:                   [接收]─→[写共享内存]
                             ↓
FSPI:                                [读共享内存]─→[判断]─→[vehicle_control]
                                                              ↓
硬件:                                                      [SPI写入]─→[电机转向]
                                                              ↓
小车:                                                      [实际转向]

←────────────────── 约100-200ms总延迟 ──────────────────→
```

---

## 性能参数

| 参数 | 值 | 说明 |
|-----|---|------|
| UDP帧率 | 30fps | M端发送频率 |
| FSPI检测周期 | 100ms | 主循环周期 |
| 置信度阈值 | 60% | 执行转向的最低置信度 |
| 转向超时 | 0.5秒 | 自动重置时间 |
| SPI写入重试 | 3次 | 失败重试次数 |
| 防抖动阈值 | 20% | 置信度变化阈值 |

---

## 调试方法

### 1. 查看共享内存状态
```bash
# 查看所有共享内存
ipcs -m

# 应该看到：
# key        shmid      owner      perms      bytes
# 0x1234567e 12345      root       666        72
```

### 2. 监控UDP接收
S端UDP程序会打印：
```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  [路线识别] 帧 1234
  弯道类型: ↰↰ 左弯
  置信度:   85%
  弯道角度: 30 度
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 3. 监控FSPI控制
FSPI程序会打印：
```
🛣️  [路况监控] 帧1234: 左弯 (置信度:85%, 角度:30°)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🛣️  路况变化检测 - 帧1234
   ⚡ 启动左转 (置信度: 85%, 角度: 30°)
   ✓ 左转已激活
正在运动 左转...
调试信息: 命令=2, enable=1, 地址=0x0002, 发送值=0
车辆控制成功: 左转 运动 (地址=0x0002, 值=0)
```

### 4. 验证硬件响应
检查FPGA/硬件寄存器：
- 寄存器2（左转）应该为0（运动中）
- 寄存器3（右转）应该为1（已停止）

---

## 常见问题

### Q1: 车辆不转向？
**检查项：**
1. 置信度是否 ≥ 60%？
2. UDP程序是否正常接收？
3. 共享内存是否创建成功？
4. FSPI程序是否读取到弯道信息？

### Q2: 车辆转向异常频繁？
**原因：** 弯道检测不稳定
**解决：** 提高置信度阈值（修改第562行和596行）

### Q3: 转向反应慢？
**原因：** FSPI检测周期太长
**解决：** 减小usleep(100000)的值（第627行）

---

## 总结

整个控制链路的核心是：
1. **UDP传输弯道信息** - 通过帧头携带
2. **共享内存中转** - 解耦网络接收和控制逻辑
3. **智能决策** - 基于置信度和状态机
4. **硬件控制** - 通过FSPI/SPI接口
5. **安全保护** - 多重机制确保稳定性

关键设计亮点：
- ✅ 低延迟（<200ms）
- ✅ 高可靠性（重试机制）
- ✅ 安全保护（置信度阈值、互斥控制、超时保护）
- ✅ 易于调试（详细日志）
