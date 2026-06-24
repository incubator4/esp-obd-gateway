# board/c6/

[Waveshare ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3) 的 UI 注册与专用界面。

## 硬件概要

- 240×240 ST7789V2（SPI）
- 无电容触摸；**BOOT** 键（GPIO9）用于切屏
- 可选：TF 卡、RGB LED（HAL 尚未封装）

引脚：`include/config_display_c6.h`  
HAL：`lib/display/panels/c6_lcd_13/`

## 当前注册的 Screen（BOOT 短按循环）

| 顺序 | Screen | 说明 |
|------|--------|------|
| 1 | 转速 RPM | 全屏弧形仪表 |
| 2 | 时速 | 全屏弧形仪表 |
| 3 | Settings | 设置占位 |

底部显示 `页码/总数  BOOT>`，例如 `1/3  BOOT>`。

## 扩展建议（screens/）

可在本目录新增 C6 专用 Screen，例如 RGB 灯效、SD 卡状态。在 `register.cpp` 中按期望顺序 `nav.addScreen(&g_xxx)` 即可插入 BOOT 循环。

## API

```cpp
void registerC6Screens(ui::Navigator& nav, const disp::PanelSize& panel_size);
```

构建：`pio run -e display_c6`
