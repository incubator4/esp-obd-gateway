# esp-obd-gateway

ESP32 汽车 OBD-II 数据网关：4D Systems GEN4 ESP32-S3 从 CAN 读数据，经 **ESP-NOW** 发到 **ESP32-C6 + ST7789V2** 显示端，用 **LVGL** 绘制界面。

## 架构

```
┌──────────────────────────────┐         ESP-NOW          ┌─────────────────────────────┐
│  Gateway（服务端）              │  ── 广播 Telemetry ──►  │  Display（接收端）             │
│  4D Systems GEN4 ESP32-S3      │  ◄── PID 请求 / Ping ──  │  ESP32-C6 + ST7789V2 SPI     │
│  TWAI CAN ↔ OBD（不用板载屏）   │                          │  LVGL 320×170 横屏仪表盘       │
└──────────────────────────────┘                          └─────────────────────────────┘
```

| 环境      | 板型                            | 职责                            |
| --------- | ------------------------------- | ------------------------------- |
| `gateway` | `4d_systems_esp32s3_gen4_r8n16` | CAN 收发、OBD PID、ESP-NOW 广播 |
| `display` | `esp32-c6-devkitm-1` + ST7789V2 | ESP-NOW 接收、SPI 屏 + LVGL     |

## 硬件接线

### Gateway（4D GEN4-S3）

- 通过 FFC 扩展口接 **3.3V CAN 收发器**（如 SN65HVD230）
- 默认 TWAI：`GPIO17` = TX，`GPIO18` = RX（见 `include/config_gateway.h`）
- OBD-II 500 kbps（老车可改 250 kbps）

### Display（ESP32-C6 + 板载 ST7789V2）

默认 SPI 引脚（`include/config_display.h`，按原理图修改）：

| 信号 | GPIO                     |
| ---- | ------------------------ |
| MOSI | 4                        |
| SCLK | 5                        |
| DC   | 6                        |
| CS   | 7                        |
| RST  | 14                       |
| BL   | 15（LOW = 亮，反相背光） |

分辨率：物理 170×320，UI 横屏 **320×170**。

## 构建

```bash
pio run -e gateway    # 刷到 4D GEN4-S3 网关
pio run -e display    # 刷到 ESP32-C6 显示端
```

Display 环境使用 **ESP-IDF**（PlatformIO 官方支持 C6）。若需 Arduino，需自行安装 Arduino-ESP32 3.x 工具链。

## 配置

- 网关 CAN / ESP-NOW：`include/config_gateway.h`
- 显示 SPI / ESP-NOW：`include/config_display.h`
- 协议定义：`include/protocol.h`

## 数据协议

| 消息                    | 方向        | 说明        |
| ----------------------- | ----------- | ----------- |
| `MSG_TELEMETRY`         | 网关 → 广播 | ~10 Hz 快照 |
| `MSG_PID_REQUEST`       | 显示 → 网关 | 按需查 PID  |
| `MSG_PID_RESPONSE`      | 网关 → 显示 | 单播应答    |
| `MSG_PING` / `MSG_PONG` | 双向        | 链路检测    |

两设备须使用 **相同 Wi-Fi 信道**（默认 channel 1）。

## 库目录

见 [`lib/README`](lib/README)。简要结构：

```
lib/transport/   ESP-NOW 传输（crc / packet / espnow）
lib/obd/         CAN → ISO-TP → 诊断 → 采集
include/         协议与板级配置
```
