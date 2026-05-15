# OV-Watch ↔ ESP32-S3 串口通信协议使用手册

## 1. 概述

OV-Watch 通过 UART1 与 ESP32-S3 建立二进制帧通信链路，实现健康数据上报、PPG波形流传输、SOS紧急报警、时间同步、语音助手唤醒等功能。

### 1.1 硬件连接

| OV-Watch (STM32F411) | ESP32-S3 | 说明 |
|----------------------|----------|------|
| PA9  (UART1_TX)      | GPIO_RX  | STM32发送 → ESP32接收 |
| PA10 (UART1_RX)      | GPIO_TX  | ESP32发送 → STM32接收 |
| GND                  | GND      | 共地 |

### 1.2 串口参数

| 参数     | 值         |
|----------|-----------|
| 波特率   | 115200    |
| 数据位   | 8         |
| 停止位   | 1         |
| 校验     | 无        |
| 流控     | 无        |

---

## 2. 帧格式

所有数据帧采用统一的二进制格式：

```
┌──────┬──────┬──────────┬─────────────┬──────┐
│ HEAD │ TYPE │ LEN(L,H) │   PAYLOAD   │ CHK  │
│ 1B   │ 1B   │   2B     │  0~512B     │ 1B   │
└──────┴──────┴──────────┴─────────────┴──────┘
```

| 字段    | 长度 | 说明                                      |
|---------|------|-------------------------------------------|
| HEAD    | 1B   | 帧头，固定 `0xAA`                         |
| TYPE    | 1B   | 帧类型（见下表）                          |
| LEN     | 2B   | PAYLOAD长度，小端序（低位在前）           |
| PAYLOAD | 0~512B | 数据载荷（长度由LEN指定）              |
| CHK     | 1B   | 校验字节 = TYPE ⊕ LEN_L ⊕ LEN_H ⊕ 所有PAYLOAD字节 |

**帧头开销**: 5字节（HEAD + TYPE + LEN + CHK）

### 2.1 校验计算示例

```c
uint8_t chk = type ^ (len & 0xFF) ^ ((len >> 8) & 0xFF);
for(int i = 0; i < len; i++)
    chk ^= payload[i];
```

### 2.2 接收解析流程

```
1. 在串口接收缓冲区中搜索 0xAA 帧头
2. 读取 TYPE(1B) 和 LEN(2B, 小端)
3. 等待接收 LEN + 1 字节（payload + chk）
4. 校验 CHK，失败则丢弃该帧，回到步骤1
5. 校验通过，根据 TYPE 分发处理
```

**容错**: 未知TYPE按LEN跳过payload，不影响后续帧解析。

---

## 3. 帧类型定义

### 3.1 STM32 → ESP32（数据上报）

| TYPE | 名称     | PAYLOAD                                      | 长度  |
|------|----------|----------------------------------------------|-------|
| 0x01 | 健康数据 | hr, spo2, sbp, dbp, fatigue, micro, resp, rmssd | 8B   |
| 0x02 | PPG波形  | ppg[0..63]（有符号int16, 小端）              | 128B  |
| 0x03 | 环境数据 | temp, humi, alt_hi, alt_lo, dir_hi, dir_lo   | 6B    |
| 0x04 | 运动数据 | steps_hi, steps_lo, battery                  | 3B    |
| 0x05 | SOS报警  | abnormal_type（bitmask）                     | 1B    |
| 0x06 | 时间同步 | year, month, date, hour, min, sec            | 6B    |
| 0x86 | 语音唤醒 | 无                                           | 0B    |

#### 0x01 健康数据（8字节）

| 偏移 | 字段    | 类型     | 范围      | 说明           |
|------|---------|----------|-----------|----------------|
| 0    | hr      | uint8    | 40~200    | 心率 (bpm)     |
| 1    | spo2    | uint8    | 70~99     | 血氧饱和度 (%) |
| 2    | sbp     | uint8    | 60~200    | 收缩压 (mmHg)  |
| 3    | dbp     | uint8    | 40~130    | 舒张压 (mmHg)  |
| 4    | fatigue | uint8    | 0~100     | 疲劳指数        |
| 5    | micro   | uint8    | 0~100     | 微循环指数      |
| 6    | resp    | uint8    | 8~40      | 呼吸率 (次/分)  |
| 7    | rmssd   | uint8    | 0~200     | HRV-RMSSD (ms) |

#### 0x02 PPG波形（128字节）

| 偏移      | 字段           | 类型     | 说明                    |
|-----------|----------------|----------|-------------------------|
| 0~1       | ppg[0]         | int16 LE | 第0个采样点             |
| 2~3       | ppg[1]         | int16 LE | 第1个采样点             |
| ...       | ...            | ...      | ...                     |
| 126~127   | ppg[63]        | int16 LE | 第63个采样点            |

共64个有符号16位采样点，小端序。采样率约50Hz，单帧覆盖约1.28秒。

#### 0x03 环境数据（6字节）

| 偏移 | 字段     | 类型     | 说明                    |
|------|----------|----------|-------------------------|
| 0    | temp     | uint8    | 温度 (°C)               |
| 1    | humi     | uint8    | 湿度 (%)                |
| 2    | alt_hi   | uint8    | 海拔高字节              |
| 3    | alt_lo   | uint8    | 海拔低字节              |
| 4    | dir_hi   | uint8    | 方位角高字节            |
| 5    | dir_lo   | uint8    | 方位角低字节            |

海拔 = (alt_hi << 8) | alt_lo，单位：米
方位角 = (dir_hi << 8) | dir_lo，单位：度

#### 0x04 运动数据（3字节）

| 偏移 | 字段      | 类型     | 说明             |
|------|-----------|----------|------------------|
| 0    | steps_hi  | uint8    | 步数高字节       |
| 1    | steps_lo  | uint8    | 步数低字节       |
| 2    | battery   | uint8    | 电池电量 (%)     |

步数 = (steps_hi << 8) | steps_lo

#### 0x05 SOS报警（1字节）

| 偏移 | 字段          | 类型     | 说明                    |
|------|---------------|----------|-------------------------|
| 0    | abnormal_type | uint8    | 异常类型bitmask          |

异常类型编码（可组合）：

| Bit | 宏定义         | 含义         | 显示文字   |
|-----|---------------|-------------|-----------|
| 0   | SOS_TYPE_FALL  | 跌倒检测     | 跌倒      |
| 1   | SOS_TYPE_HR    | 心率异常     | 心率异常  |
| 2   | SOS_TYPE_SPO2  | 血氧异常     | 血氧异常  |
| 3   | SOS_TYPE_MICRO | 微循环异常   | 微循环异常|
| 4   | SOS_TYPE_BP    | 血压异常     | 血压异常  |
| 5   | SOS_TYPE_HRV   | HRV异常      | HRV异常   |
| 6   | SOS_TYPE_FATIGUE | 疲劳异常   | 疲劳异常  |

示例：`0x03` = bit0+bit1 = 跌倒 + 心率异常

#### 0x06 时间同步（6字节）

| 偏移 | 字段   | 类型     | 范围      |
|------|--------|----------|-----------|
| 0    | year   | uint8    | 0~99      |
| 1    | month  | uint8    | 1~12      |
| 2    | date   | uint8    | 1~31      |
| 3    | hour   | uint8    | 0~23      |
| 4    | min    | uint8    | 0~59      |
| 5    | sec    | uint8    | 0~59      |

year=24 表示 2024年。

#### 0x86 语音唤醒（0字节）

无PAYLOAD。STM32双击按键时发送，通知ESP32启动语音助手。

---

### 3.2 ESP32 → STM32（命令下发）

| TYPE | 名称       | PAYLOAD              | 说明                   |
|------|------------|----------------------|------------------------|
| 0x80 | 心跳       | 无                   | ESP32在线检测，STM32回复运动+环境 |
| 0x81 | 请求健康   | 无                   | 触发STM32发送一次健康数据帧     |
| 0x82 | PPG控制    | 1B: 0=停, 1=启       | 开关PPG波形流           |
| 0x83 | 设置时间   | 6B: 年月日时分秒     | 校准STM32的RTC时钟      |
| 0x84 | 查询状态   | 无                   | STM32回复运动+环境+时间 |
| 0x85 | 语音状态   | 1B: 0=关, 1=开       | 通知STM32语音状态变化   |

#### 0x80 心跳

ESP32定期发送（建议每5~10秒）。STM32收到后回复运动+环境数据帧。
可用于判断ESP32是否在线。

#### 0x82 PPG控制（1字节）

| 偏移 | 字段 | 值  | 说明         |
|------|------|-----|-------------|
| 0    | ctrl | 0   | 停止PPG流   |
|      |      | 1   | 启动PPG流   |

启动后STM32以约5Hz（200ms间隔）持续发送0x02 PPG波形帧，直到收到停止命令。

#### 0x83 设置时间（6字节）

格式同0x06时间同步帧的PAYLOAD。STM32收到后校验合法性并设置RTC。

#### 0x85 语音状态（1字节）

| 偏移 | 字段   | 值  | 说明         |
|------|--------|-----|-------------|
| 0    | voice  | 0   | 语音关闭     |
|      |        | 1   | 语音开启     |

---

## 4. 数据上报策略

| 数据类型 | 触发方式       | 频率/条件                |
|----------|---------------|--------------------------|
| 运动+环境 | 定时          | 每1秒                    |
| 健康数据  | 事件驱动       | MKS142每10分钟测量稳定后  |
| PPG波形   | ESP32请求后流式 | 约5Hz（200ms/帧）        |
| SOS报警   | 事件驱动       | 跌倒或健康指标异常时      |
| 时间同步  | ESP32请求      | 按需                     |
| 语音唤醒  | 按键双击       | 每次双击触发              |

---

## 5. ESP32端开发指南

### 5.1 帧解析参考代码（C语言）

```c
#define COMM_FRAME_HEAD     0xAA
#define COMM_FRAME_OVERHEAD 5

typedef enum {
    COMM_TYPE_HEALTH    = 0x01,
    COMM_TYPE_PPG       = 0x02,
    COMM_TYPE_ENV       = 0x03,
    COMM_TYPE_MOTION    = 0x04,
    COMM_TYPE_SOS       = 0x05,
    COMM_TYPE_TIME      = 0x06,
    COMM_TYPE_RESPONSE  = 0x10,
    COMM_TYPE_WAKE      = 0x86,
    // ESP32 → STM32
    COMM_TYPE_HEARTBEAT = 0x80,
    COMM_TYPE_REQ_HEALTH = 0x81,
    COMM_TYPE_PPG_CTRL  = 0x82,
    COMM_TYPE_SET_TIME  = 0x83,
    COMM_TYPE_QUERY     = 0x84,
    COMM_TYPE_VOICE     = 0x85,
} comm_type_t;

// 发送一帧
void comm_send(uart_port_t uart, uint8_t type, const uint8_t *payload, uint16_t len) {
    uint8_t buf[5 + 512];
    buf[0] = COMM_FRAME_HEAD;
    buf[1] = type;
    buf[2] = len & 0xFF;
    buf[3] = (len >> 8) & 0xFF;
    uint8_t chk = type ^ buf[2] ^ buf[3];
    for (int i = 0; i < len; i++) {
        buf[4 + i] = payload[i];
        chk ^= payload[i];
    }
    buf[4 + len] = chk;
    uart_write_bytes(uart, buf, 5 + len);
}

// 解析接收到的帧
void comm_parse(const uint8_t *buf, uint16_t len) {
    if (len < 5 || buf[0] != 0xAA) return;
    uint8_t type = buf[1];
    uint16_t plen = buf[2] | (buf[3] << 8);
    if (len < 5 + plen) return;

    // 校验
    uint8_t chk = type ^ buf[2] ^ buf[3];
    for (int i = 0; i < plen; i++) chk ^= buf[4 + i];
    if (chk != buf[4 + plen]) return;

    const uint8_t *p = &buf[4];
    switch (type) {
        case COMM_TYPE_HEALTH:
            // p[0]=hr, p[1]=spo2, p[2]=sbp, p[3]=dbp,
            // p[4]=fatigue, p[5]=micro, p[6]=resp, p[7]=rmssd
            break;
        case COMM_TYPE_PPG:
            // 64个int16_t小端: p[0..127]
            break;
        case COMM_TYPE_MOTION:
            // steps = (p[0]<<8)|p[1], battery = p[2]
            break;
        case COMM_TYPE_SOS:
            // abnormal_type = p[0] (bitmask)
            break;
        // ... 其他类型
    }
}
```

### 5.2 常用操作示例

**请求健康数据：**
```c
comm_send(uart, 0x81, NULL, 0);
// 等待STM32回复 0x01 健康数据帧
```

**启动PPG流：**
```c
uint8_t ctrl = 1;
comm_send(uart, 0x82, &ctrl, 1);
// STM32将以~5Hz持续发送 0x02 PPG帧
```

**校准时间：**
```c
uint8_t time[6] = {25, 5, 13, 14, 30, 0}; // 2025-05-13 14:30:00
comm_send(uart, 0x83, time, 6);
```

**心跳保活：**
```c
// 每5~10秒发送一次
comm_send(uart, 0x80, NULL, 0);
// STM32回复运动+环境数据
```

### 5.3 SOS报警处理

收到 0x05 帧时，解析 `abnormal_type` bitmask：

```c
if (type == COMM_TYPE_SOS) {
    uint8_t abnormal = payload[0];
    if (abnormal & 0x01) /* 跌倒 */;
    if (abnormal & 0x02) /* 心率异常 */;
    if (abnormal & 0x04) /* 血氧异常 */;
    if (abnormal & 0x08) /* 微循环异常 */;
    if (abnormal & 0x10) /* 血压异常 */;
    if (abnormal & 0x20) /* HRV异常 */;
    if (abnormal & 0x40) /* 疲劳异常 */;
    // 执行报警: 蜂鸣器、LED闪烁、上报云端等
}
```

---

## 6. 语音助手唤醒流程

```
用户双击手表按键
    ↓
STM32 KeyTask 检测到双击 (keystr=3 或 4)
    ↓
写入 VoiceKey_MessageQueue
    ↓
CommTask 检测到队列消息
    ↓
发送 0x86 语音唤醒帧 (无payload)
    ↓
ESP32 收到 0x86 → 启动语音识别
```

ESP32端收到 0x86 后应启动语音采集和识别流程。

---

## 7. 调试方法

### 7.1 串口抓包

使用串口助手（如SSCOM、PuTTY）连接UART1（115200 8N1），可观察到：

- **STM32发出的帧**（hex模式）：
  - 每秒: `AA 04 03 00 xx xx xx yy` (运动) + `AA 03 06 00 ...` (环境)
  - 测量后: `AA 01 08 00 xx xx xx xx xx xx xx xx yy` (健康)
  - 双击: `AA 86 00 00 86` (语音唤醒)

- **发送给STM32的帧**：
  - 心跳: `AA 80 00 00 80`
  - 请求健康: `AA 81 00 00 81`
  - 开PPG: `AA 82 01 00 01 83`

### 7.2 printf调试输出

STM32的printf输出会与二进制帧混合在UART1上。关键调试信息：

```
[SOS] trigger type=0x01        ← SOS触发（含异常类型）
[COMM] voice wake              ← 语音唤醒帧已发送
DT:acc=101 roll=-545 pitch=-838 state=CONFIRMED  ← 跌倒检测数据
```

> **注意**: 正式发布时建议禁用printf，避免与二进制协议数据冲突。

### 7.3 常见问题

| 现象 | 原因 | 解决 |
|------|------|------|
| ESP32收不到数据 | 接线错误或波特率不匹配 | 检查TX/RX交叉连接，确认115200 |
| 帧校验失败 | 数据传输错误 | 检查接线、降低干扰 |
| PPG数据全0 | MKS142未启动测量 | 先发送0x82 PPG控制命令启动 |
| 健康数据为0 | 尚未完成一次测量 | 等待10分钟自动测量或手动测量 |
| SOS帧未发送 | CommTask未运行 | 检查FreeRTOS堆内存是否充足 |

---

## 8. 文件清单

| 文件路径 | 说明 |
|----------|------|
| `User/Func/Inc/comm_protocol.h` | 协议定义、帧类型常量、API声明 |
| `User/Func/Src/comm_protocol.c` | 帧收发实现、数据打包、命令解析 |
| `User/Tasks/Inc/user_CommTask.h` | 通信任务声明 |
| `User/Tasks/Src/user_CommTask.c` | 通信主任务（定时上报、接收处理、语音唤醒） |

---

## 9. 扩展指南

### 9.1 添加新的帧类型

1. 在 `comm_protocol.h` 中定义新的 TYPE 常量（如 `0x07`）
2. 在 `comm_protocol.c` 中添加 `Comm_SendXxx()` 函数
3. 在 `Comm_ParseFrame()` 的 switch 中添加新的 case
4. 更新本手册的帧类型表

### 9.2 修改PAYLOAD格式

修改对应的 `Comm_SendXxx()` 函数中的打包逻辑和长度即可，帧格式自动适配。

### 9.3 调整上报频率

在 `user_CommTask.c` 中修改宏定义：

```c
#define COMM_MOTION_ENV_INTERVAL_MS  1000  /* 运动+环境上报间隔 */
#define COMM_PPG_INTERVAL_MS         200   /* PPG波形上报间隔 */
```
