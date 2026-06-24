# board/s3/

[Waveshare ESP32-S3-Touch-LCD-1.69](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69) 的 UI 注册与专用界面。

## 硬件概要

- 240×280 ST7789V2 + CST816T 电容触摸
- QMI8658 六轴 IMU（I2C `0x6B`）
- PCF85063 RTC、ETA6098 充电、GPIO 蜂鸣器等（UI/HAL 待扩展）

引脚：`include/config_display_s3.h`  
HAL：`lib/display/panels/s3_touch_169/`  
IMU：`lib/display/features/imu_attitude.*`

## 当前注册的 Screen

| 顺序 | Screen | ScreenId | 说明 |
|------|--------|----------|------|
| 1 | OBD Dashboard | `ObdDashboard` | 通用 |
| 2 | Attitude Horizon | `AttitudeHorizon` | **S3 专用**，飞机姿态 / 人工地平仪 |
| 3 | Settings | `Settings` | 通用 |

输入：触摸转发到当前 Screen；BOOT 下一屏；PWR（SYS_OUT）上一屏。

## screens/attitude

人工地平仪界面，读取 `ImuAttitudeFeature` 的 roll/pitch/yaw（融合算法 TODO）。

```
FeatureRegistry.updateAll()
       → ImuAttitudeFeature::update()
       → AttitudeHorizonScreen::onTick()  → LVGL 绘制（TODO）
```

触摸预留：yaw 校准 / 重置。

## 扩展建议

| 设想界面 | 板载资源 |
|----------|----------|
| 时钟 | PCF85063 RTC |
| 电池 | BAT_ADC (GPIO1) |
| 蜂鸣器测试 | GPIO42 |

新增步骤同 C6：在 `screens/` 加类 → `register.cpp` 注册 → 必要时在 `lib/display/features/` 加 HAL。

## API

```cpp
void registerS3Screens(ui::Navigator& nav);
ui::ObdDashboardScreen* s3ObdDashboardScreen();
```

构建：`pio run -e display_s3`
