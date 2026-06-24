# app/screens/

**跨板型共用**的 Screen 实现，所有 display env 都会编译。

## 文件

| 文件 | 类 | 说明 |
|------|-----|------|
| `common.h` / `common.cpp` | `ObdDashboardScreen` | 展示 ESP-NOW 收到的 `ObdTelemetry` |
| | `SettingsScreen` | 设置占位 |

## ObdDashboardScreen

- `setTelemetry(const ObdTelemetry*, bool stale)` — 由 `main.cpp` 每帧调用
- `onEnter()` / `onTick()` — 后续创建 LVGL 标签、仪表盘控件
- `onTouch()` — 可选：点击切换 PID 详情

## 与 board/ 的关系

| 类型 | 放置位置 |
|------|----------|
| 所有板都有 | 本目录 `app/screens/` |
| 仅 C6 / 仅 S3 | `board/c6/screens/` 或 `board/s3/screens/` |

注册仍在各板的 `register.cpp` 中 `nav.addScreen(&g_obd)`。
