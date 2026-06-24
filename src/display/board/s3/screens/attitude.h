#pragma once

#include "app/screen.h"

namespace ui {

/**
 * S3-Touch-LCD-1.69 专用：六轴 IMU 人工地平仪（飞机姿态）。
 * 依赖 lib/display/features/imu_attitude.h 提供 roll/pitch/yaw。
 */
class AttitudeHorizonScreen : public Screen {
public:
    ScreenId id() const override { return ScreenId::AttitudeHorizon; }
    const char* title() const override { return "Attitude"; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;
    void onTouch(int16_t x, int16_t y) override;

private:
    void drawHorizonPlaceholder(float roll_deg, float pitch_deg, bool valid);
};

}  // namespace ui
