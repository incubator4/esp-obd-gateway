# src/display

显示端**应用固件**：ESP-NOW 收遥测、界面导航、LVGL 界面（绘制部分待实现）。

硬件驱动在 [`lib/display/`](../../lib/display/README.md)。

## 目录结构

```
src/display/
├── main.cpp              # setup/loop：HAL + Navigator + ESP-NOW
├── app/                  # 通用 UI 框架
│   ├── screen.h          # Screen 基类、ScreenId
│   ├── navigator.*       # 切屏、输入分发
│   └── screens/          # 各板共用的界面（OBD、设置）
└── board/                # 板级 Screen 注册
    ├── c6/               # C6 专用（当前仅注册通用屏）
    └── s3/               # S3 专用（含 Attitude 姿态屏）
        └── screens/
```

## 主循环（main.cpp）

```
setup:
  createPanel / createInput → begin
  registerFeatures → beginAll
  registerC6Screens / registerS3Screens
  EspNow begin + onReceive

loop:
  features.updateAll()
  nav.handleInput()    ← BOOT 下一屏 / PWR 上一屏 / 触摸
  nav.tick()
  telemetryRegistry().setAll(...)   ← 更新当前已注册 Screen 的遥测
```

## 构建环境

| 命令 | 板型 | 编译的 board 目录 |
|------|------|-------------------|
| `pio run -e display_c6` | C6-LCD-1.3 | `board/c6/` |
| `pio run -e display_s3` | S3-Touch-LCD-1.69 | `board/s3/` |

## ScreenId 约定

| ID | 界面 | 板型 |
|----|------|------|
| `Rpm` | 发动机转速 | 通用 |
| `Speed` | 当前时速 | 通用 |
| `Settings` | 设置 | 通用 |
| `AttitudeHorizon` (0x80) | IMU 人工地平仪 | 仅 S3 |

通用 ID 使用低字节；板专用 ID 建议从 `0x80` 起，避免冲突。

## 扩展指南

1. **所有板都要的界面** → `app/screens/` + 在各 `board/*/register.cpp` 里 `nav.addScreen`
2. **仅某块板** → `board/<chip>/screens/` + 仅在该板 `register.cpp` 注册
3. **读板载传感器** → 通过 `lib/display` 的 `FeatureRegistry`，不要在 Screen 里直接操作 I2C
