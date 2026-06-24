# features/

板载**扩展能力**实现，通过 `core/feature.h` 的 `Feature` 接口注册。

与 `panels/` 的区别：

- `panels/`：所有板几乎都需要（屏 + 基本输入）
- `features/`：仅部分板有（IMU、RTC、蜂鸣器等）

## 现有特性

| 文件 | FeatureId | 硬件 | 说明 |
|------|-----------|------|------|
| `imu_attitude.*` | `ImuAttitude` | S3 板载 QMI8658 | 输出 `ImuAttitude`（roll/pitch/yaw），供人工地平仪 UI |

S3 在 `board/factory.cpp` 中注册；C6 无 IMU，不注册任何 Feature。

## 数据流

```
ImuAttitudeFeature::update()   ← I2C 读 QMI8658（TODO）
        │
        ▼
AttitudeHorizonScreen::onTick()   ← src/display/board/s3/screens/
        │
        ▼
LVGL 地平线控件（TODO）
```

## 新增特性

1. 继承 `Feature`，实现 `id()` / `begin()` / `update()` / `available()`
2. 在对应板的 `board/factory.cpp` 里 `registry.add(...)`
3. 若需 UI，在 `src/display/board/<chip>/screens/` 增加 Screen

示例（预留）：`RtcClockFeature`、`BatteryFeature`、`BuzzerFeature`（S3 板载 ETA6098 / PCF85063 / GPIO42）。
