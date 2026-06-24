# board/c6/

[Waveshare ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3) 的 UI 注册与专用界面。

## 硬件概要

- 240×240 ST7789V2（SPI）
- 无电容触摸；**BOOT** 键用于切屏
- 可选：TF 卡、RGB LED（HAL 尚未封装）

引脚：`include/config_display_c6.h`  
HAL：`lib/display/panels/c6_lcd_13/`

## 当前注册的 Screen

| 顺序 | Screen | 说明 |
|------|--------|------|
| 1 | OBD Dashboard | 通用，`app/screens/common` |
| 2 | Settings | 通用 |

BOOT 短按在 `Navigator` 中循环切换以上界面。

## 扩展建议（screens/）

可在本目录新增 C6 专用 Screen，例如：

| 设想界面 | 依赖硬件 |
|----------|----------|
| RGB 灯效 | 板载 RGB（需 HAL Feature） |
| SD 卡状态 | TF 卡槽 |

步骤：

1. 在 `screens/` 新建 `xxx.h` / `xxx.cpp`，继承 `ui::Screen`
2. 在 `register.cpp` 中 `nav.addScreen(&g_xxx)`
3. 若需新 HAL 能力，在 `lib/display/features/` 增加 Feature 并在 C6 的 `factory.cpp` 分支注册

## API

```cpp
void registerC6Screens(ui::Navigator& nav);
ui::ObdDashboardScreen* c6ObdDashboardScreen();
```

构建：`pio run -e display_c6`
