# esp-obd-gateway

ESP32 汽车 OBD-II 数据网关：网关从 CAN 读数据，经 **ESP-NOW** 发到 Waveshare 显示板，用 **LVGL** 绘制界面。

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
| `display_c6_13` | Waveshare ESP32-C6-LCD-1.3 | ESP-NOW、240×240、BOOT 切屏 |
| `display_c6_147` | Waveshare ESP32-C6-LCD/Touch-LCD-1.47 | ESP-NOW、172×320、触摸自动探测 |
| `display_s3_169` | Waveshare ESP32-S3-LCD/Touch-LCD-1.69 | 触摸自动探测、IMU 姿态屏、ESP-NOW |

## 硬件接线

### Gateway（4D GEN4-S3）

- 通过 FFC 扩展口接 **3.3V CAN 收发器**（如 SN65HVD230）
- 默认 TWAI：`GPIO17` = TX，`GPIO18` = RX（见 `include/config_gateway.h`）
- OBD-II 500 kbps（老车可改 250 kbps）

### Display（Waveshare）

| 板型 | 环境 | 分辨率 | 输入 |
|------|------|--------|------|
| [ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3) | `display_c6_13` | 240×240 | BOOT 切屏 |
| [ESP32-C6-LCD/Touch-LCD-1.47](https://docs.waveshare.com/ESP32-C6-Touch-LCD-1.47) | `display_c6_147` | 172×320 | 触摸自动探测 / BOOT |
| [ESP32-S3-LCD/Touch-LCD-1.69](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69) | `display_s3_169` | 240×280 | 触摸自动探测 + PWR/BOOT |

引脚见 `include/display_profiles.h`（按 `DISPLAY_PROFILE_*` 选择）。

## 构建

```bash
pio run -e gateway       # 刷到 4D GEN4-S3 网关
pio run -e display_c6_13     # Waveshare C6-LCD-1.3
pio run -e display_c6_147   # Waveshare C6-LCD/Touch-LCD-1.47（touch 自动探测）
pio run -e display_s3_169   # Waveshare S3-LCD/Touch-LCD-1.69（touch 自动探测）

# 或统一入口
make flash-display BOARD=c6_147 PORT=/dev/cu.usbmodem101
```

显示端使用 **Arduino** 框架（两块 Waveshare 板均官方支持）。

## CI / Release

- **CI**：推送到 `main` 或 PR 时自动编译全部 PlatformIO 环境（见 [`.github/workflows/ci.yml`](.github/workflows/ci.yml)）。
- **Release**：推送 `v*` 标签（如 `v1.0.0`）时构建固件并发布到 GitHub Releases。

```bash
# 本地打包（与 CI release 相同产物）
make release

# 发布新版本
git tag v1.0.0
git push origin v1.0.0
```

Release 包内容：

| 文件 | 说明 |
|------|------|
| `gateway.bin` | 4D GEN4-S3 网关 |
| `display-c6-13.bin` | C6 1.3" 显示端 |
| `display-c6-147.bin` | C6 1.47" 显示端（touch 自动探测） |
| `display-s3-169.bin` | S3 1.69" 显示端（touch 自动探测） |
| `manifest.json` | 版本、SHA256、板型元数据 |
| `FLASHING.md` | 刷写说明 |

也可在 GitHub Actions 中手动触发 **Release** workflow 指定版本号。

## 配置

- 网关 CAN / ESP-NOW：`include/config_gateway.h`
- 显示板配置：`include/display_profiles.h`
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

见 [`lib/README`](lib/README)。显示层详细说明：

- HAL：[`lib/display/README.md`](lib/display/README.md)
- UI：[`src/display/README.md`](src/display/README.md)

简要结构：

```
lib/transport/   ESP-NOW 传输
lib/obd/         CAN → 采集
lib/display/     面板 / 输入 / 板载特性（IMU）
src/display/     UI 导航 + 各板专用界面
include/         协议与板级配置
```
