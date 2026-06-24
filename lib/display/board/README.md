# board/

编译期**板型选择**：根据 `DISPLAY_BOARD_*` 宏创建正确的 Panel / Input / Feature 集合。

## 文件

| 文件 | 说明 |
|------|------|
| `factory.h` | 声明 `createPanel()`、`createInput()`、`registerFeatures()` |
| `factory.cpp` | `#if defined(DISPLAY_BOARD_...)` 分支，绑定具体类实例 |

## 编译宏

由 `platformio.ini` 各 display env 注入：

```ini
; display_c6
-D DISPLAY_BOARD_C6_LCD_13

; display_s3
-D DISPLAY_BOARD_S3_TOUCH_169
```

同时 `-include config_display_*.h` 提供引脚宏。

## 与 src/display 的对应

| HAL（本目录） | UI（src/display/board/） |
|---------------|--------------------------|
| `DISPLAY_BOARD_C6_LCD_13` | `board/c6/register.cpp` |
| `DISPLAY_BOARD_S3_TOUCH_169` | `board/s3/register.cpp` |

两处宏必须一致；PIO 的 `build_src_filter` 只编译对应 `board/c6` 或 `board/s3`，避免链接未使用的 Screen。

## 注意

`factory.cpp` 使用**静态全局对象**（如 `static C6Lcd13Panel g_panel`），保证 ESP-NOW 回调等场景下实例地址稳定。
