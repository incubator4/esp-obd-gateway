#include "board/s3/screens/attitude.h"

#include <Arduino.h>

#include "features/imu_attitude.h"

namespace ui {

void AttitudeHorizonScreen::onEnter() {
    Serial.println("[UI] Attitude horizon (IMU)");
}

void AttitudeHorizonScreen::onExit() {}

void AttitudeHorizonScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    // TODO: 从 disp::FeatureRegistry 取 ImuAttitudeFeature，刷新 LVGL 地平线控件
    drawHorizonPlaceholder(0.0f, 0.0f, false);
}

void AttitudeHorizonScreen::onTouch(int16_t x, int16_t y) {
    (void)x;
    (void)y;
    // TODO: 触摸校准 / 重置 yaw
}

void AttitudeHorizonScreen::drawHorizonPlaceholder(float roll_deg, float pitch_deg,
                                                    bool valid) {
    (void)roll_deg;
    (void)pitch_deg;
    (void)valid;
}

}  // namespace ui
