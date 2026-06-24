# board/s3/

[Waveshare ESP32-S3-Touch-LCD-1.69](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69) 的 UI 注册与专用界面。

## 硬件概要

- 240×280 ST7789V2 + CST816T 电容触摸
- QMI8658 六轴 IMU（I2C `0x6B`）
- PCF85063 RTC、ETA6098 充电、GPIO 蜂鸣器等（UI/HAL 待扩展）

引脚：`include/config_display_s3.h`  
HAL：`lib/display/panels/s3_touch_169/`  
IMU：`lib/display/features/imu_attitude.*`

## 当前注册的 Screen（BOOT / PWR 切屏）

| 顺序 | Screen | ScreenId | 说明 |
|------|--------|----------|------|
| 1 | 转速 RPM | `Rpm` | 通用，全屏仪表 |
| 2 | 时速 | `Speed` | 通用，全屏仪表 |
| 3 | Attitude Horizon | `AttitudeHorizon` | **S3 专用**，IMU 姿态 |
| 4 | Settings | `Settings` | 通用 |

- **BOOT** 短按：下一屏
- **PWR**（SYS_OUT）短按：上一屏
- 触摸：转发到当前 Screen 的 `onTouch()`

## screens/attitude

人工地平仪界面，读取 `ImuAttitudeFeature` 的 roll/pitch/yaw（融合算法 TODO）。

## API

```cpp
void registerS3Screens(ui::Navigator& nav, const disp::PanelSize& panel_size);
```

构建：`pio run -e display_s3`
