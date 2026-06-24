# panels/

各 Waveshare 开发板的 **Panel + Input** 实现，一板一目录。

## 目录

| 目录 | 板型 | 屏 | 输入 |
|------|------|-----|------|
| `c6_lcd_13/` | [ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3) | 240×240 ST7789V2 SPI | BOOT（GPIO9） |
| `s3_touch_169/` | [ESP32-S3-Touch-LCD-1.69](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69) | 240×280 ST7789V2 SPI | CST816T 触摸 + BOOT + PWR |

每个目录通常包含：

```
panel.h    — C6Lcd13Panel / S3Touch169Panel + 对应 Input 类声明
panel.cpp  — 引脚初始化、flush 占位、触摸/按键轮询
```

## 引脚配置

不在此目录硬编码业务常量，统一从 `include/config_display_*.h` 读取，例如：

- C6：`DISP_C6_LCD_MOSI`、`DISP_C6_HOR_RES` …
- S3：`DISP_S3_LCD_DC`、`DISP_S3_I2C_SCL`、`DISP_S3_TP_ADDR` …

## 当前状态

- **C6**（`c6_lcd_13/`）：已通过 Arduino_GFX 驱动 ST7789，LVGL flush 经 `flushArea()` 输出
- **S3**（`s3_touch_169/`）：ST7789 刷屏、CST816T / QMI8658 仍为 TODO

## 新增板型

1. 新建 `panels/<板型简称>/panel.h|.cpp`
2. 在 `board/factory.cpp` 增加 `#elif defined(DISPLAY_BOARD_...)`
3. 在 `platformio.ini` 增加 env 与 `-D DISPLAY_BOARD_...`
4. 在 `src/display/board/` 增加 UI 注册（见 [`src/display/board/README.md`](../../../src/display/board/README.md)）
