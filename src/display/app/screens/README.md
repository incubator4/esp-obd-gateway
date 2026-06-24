# app/screens/

**跨板型共用**的 Screen 实现，所有 display env 都会编译。每屏只显示一项内容。

## 文件

| 文件 | 类 | 说明 |
|------|-----|------|
| `gauges.h` / `gauges.cpp` | `RpmScreen`, `SpeedScreen` | 全屏转速 / 时速 |
| `common.h` / `common.cpp` | `SettingsScreen` | 设置占位 |

## 共用组件

| 路径 | 说明 |
|------|------|
| `app/layout.h` | 根据 240×240 / 240×280 计算仪表槽位 |
| `app/widgets/value_gauge.h` | 弧形数值仪表 |
| `app/widgets/screen_chrome.h` | 标题栏、页码与 BOOT 提示 |
| `app/telemetry.h` | 遥测广播注册表 |

## 界面切换（BOOT 切屏）

- **C6**：转速 → 时速 → Settings（BOOT 短按循环）
- **S3**：转速 → 时速 → 姿态 → Settings（BOOT 下一屏 / PWR 上一屏）

底部状态栏显示页码，例如 `OBD OK  1/3  BOOT>`。

## RpmScreen / SpeedScreen

- `init(PanelLayout, ScreenNavInfo)` — 布局与页码
- `setTelemetry(...)` — 经 `TelemetryRegistry` 更新
- `onEnter()` / `onExit()` — 创建/销毁 LVGL 控件

## 与 board/ 的关系

| 类型 | 放置位置 |
|------|----------|
| 所有板都有 | 本目录 `app/screens/` |
| 仅 C6 / 仅 S3 | `board/c6/screens/` 或 `board/s3/screens/` |

注册在各板 `register.cpp` 的 `nav.addScreen(...)` 顺序即 BOOT 切屏顺序。
