# core/

显示 HAL 的抽象接口，所有板型实现均继承这些类。

## 文件

| 文件 | 说明 |
|------|------|
| `panel.h` | `Panel`：分辨率、背光、`flushArea()`（接 LVGL / GFX） |
| `input.h` | `Input`：触摸点、BOOT/PWR 等按键 |
| `feature.h` | `Feature` / `FeatureRegistry`：板载外设统一注册与轮询 |
| `feature.cpp` | `FeatureRegistry` 实现 |

## Panel

负责 ST7789 等面板的初始化与像素输出，**不**创建 LVGL 对象。

```cpp
class Panel {
    virtual bool begin() = 0;
    virtual PanelSize size() const = 0;
    virtual void setBacklight(uint8_t percent) = 0;
    virtual void flushArea(..., const uint16_t* rgb565) = 0;
};
```

## Input

统一触摸与物理键；`poll()` 在主循环调用，`buttonClicked()` 为边沿触发（单次）。

```cpp
enum class InputButton { Boot, Reset, Power };
```

## Feature

板载能力与 UI 解耦。例如 S3 的 QMI8658 通过 `ImuAttitudeFeature`（`features/imu_attitude.*`）暴露 `roll/pitch/yaw`。

```cpp
enum class FeatureId { ImuAttitude = 1 };
```

在 `board/factory.cpp` 里 `registry.add(&g_imu)`，应用层通过 `registry.get(FeatureId::ImuAttitude)` 取用。
