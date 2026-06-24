# lib/display

显示端**硬件抽象层**（HAL）：面板、输入、板载外设。不含 UI 逻辑、不含 LVGL 控件。

应用 UI 在 [`src/display/`](../../src/display/README.md)。

## 目录结构

```
lib/display/
├── display.h           # 库入口（include 此文件即可）
├── core/               # 抽象接口
├── panels/             # 各 Waveshare 板 Panel + Input 实现
├── features/           # 板载扩展能力（IMU 等）
└── board/              # 按编译宏选择板型（factory）
```

## 分层关系

```
                    display.h
                         │
         ┌───────────────┼───────────────┐
         ▼               ▼               ▼
      core/          panels/         features/
   Panel Input      具体硬件         IMU / RTC …
   Feature          ST7789 等
                         │
                         ▼
                    board/factory
              DISPLAY_BOARD_* → 创建实例
```

## 支持的板型

| 宏 | 板型 | 目录 |
|----|------|------|
| `DISPLAY_BOARD_C6_LCD_13` | Waveshare ESP32-C6-LCD-1.3 | `panels/c6_lcd_13/` |
| `DISPLAY_BOARD_S3_TOUCH_169` | Waveshare ESP32-S3-Touch-LCD-1.69 | `panels/s3_touch_169/` |

引脚与分辨率：`include/config_display_c6.h`、`include/config_display_s3.h`。

## 使用方式

```cpp
#include "display.h"

disp::Panel* panel = disp::createPanel();
disp::Input* input = disp::createInput();
disp::FeatureRegistry features;
disp::registerFeatures(features);

panel->begin();
input->begin();
features.beginAll();

// 主循环
features.updateAll();
```

## 与 src/display 的分工

| 层 | 职责 | 位置 |
|----|------|------|
| HAL | 刷屏、触摸/按键、读 IMU | `lib/display/` |
| UI | 界面、切屏、OBD 仪表盘 | `src/display/` |

新增一块 Waveshare 屏：在 `panels/` 加实现 → `board/factory.cpp` 注册 → `src/display/board/` 注册 Screen。
