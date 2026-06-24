# board/

**板级 UI 注册**：决定某块开发板上有哪些 Screen，以及板专用界面放在哪。

HAL 板型选择在 [`lib/display/board/`](../../../lib/display/board/README.md)。

## 目录

```
board/
├── c6/                 # ESP32-C6-LCD-1.3
│   ├── register.h/cpp  # registerC6Screens()
│   └── screens/        # （预留）C6 专用界面
└── s3/                 # ESP32-S3-Touch-LCD-1.69
    ├── register.h/cpp  # registerS3Screens()
    └── screens/
        └── attitude.*  # IMU 人工地平仪（AttitudeHorizon）
```

## 注册示例

```cpp
void registerS3Screens(Navigator& nav, const disp::PanelSize& panel_size) {
    nav.addScreen(&g_rpm);       // 通用
    nav.addScreen(&g_speed);     // 通用
    nav.addScreen(&g_attitude);    // S3 专用
    nav.addScreen(&g_settings);    // 通用
}
```

遥测经 `TelemetryRegistry::setAll()` 在 `main.cpp` 中广播到已注册的 Screen。

## 编译隔离

`platformio.ini` 使用 `build_src_filter` 只编译对应子树：

```ini
; display_c6
+<display/board/c6/>
-<display/board/s3/>

; display_s3
+<display/board/s3/>
-<display/board/c6/>
```

因此 S3 的 `attitude.*` 不会进入 C6 固件。

## 子目录说明

- [`c6/README.md`](c6/README.md) — C6 板 UI 扩展点
- [`s3/README.md`](s3/README.md) — S3 板 UI 与 IMU 姿态屏
