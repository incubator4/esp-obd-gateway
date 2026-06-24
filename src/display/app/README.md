# app/

显示端**通用 UI 框架**，与具体 Waveshare 板型无关。

## 文件

| 路径 | 说明 |
|------|------|
| `screen.h` | `Screen` 接口、`ScreenId` 枚举 |
| `navigator.h` / `navigator.cpp` | 多屏导航、输入路由 |
| `screens/common.h` / `common.cpp` | 通用界面实现 |

## Screen 生命周期

```
addScreen()  → 首个 Screen 自动 onEnter()
show(id)     → current onExit → next onEnter
next/prev    → 循环切换已注册 Screen
tick()       → current onTick(now_ms)
handleInput()→ poll Input → onButton / onTouch
```

每个 Screen 实现：

```cpp
virtual ScreenId id() const = 0;
virtual const char* title() const = 0;
virtual void onEnter() / onExit() / onTick() / onTouch() / onButton();
```

## Navigator 输入映射

| 输入 | 行为 |
|------|------|
| BOOT 单击 | `next()` + `onButton(Boot)` |
| PWR 单击（S3） | `prev()` + `onButton(Power)` |
| 触摸 | `onTouch(x, y)` |

## screens/common

| 类 | ScreenId | 职责 |
|----|----------|------|
| `ObdDashboardScreen` | `ObdDashboard` | 显示 `ObdTelemetry`，`setTelemetry()` 由 main 更新 |
| `SettingsScreen` | `Settings` | 设置占位 |

LVGL 控件创建放在各 Screen 的 `onEnter()`（TODO）。

## 依赖

- `disp::Input`：来自 `lib/display`，由 `Navigator::bindInput()` 注入
- `protocol.h`：`ObdTelemetry` 等数据结构

不包含 `#include "display.h"`，避免 UI 与 HAL 循环依赖。
