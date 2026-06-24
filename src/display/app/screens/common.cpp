#include "app/screens/common.h"

#include <Arduino.h>

namespace ui {

void ObdDashboardScreen::onEnter() {
    // TODO: LVGL 创建 RPM / 车速等控件
    Serial.println("[UI] OBD dashboard");
}

void ObdDashboardScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    // TODO: 刷新 telem_ 到 LVGL
}

void ObdDashboardScreen::onTouch(int16_t x, int16_t y) {
    (void)x;
    (void)y;
    // TODO: 点击区域切换 PID 详情
}

void ObdDashboardScreen::setTelemetry(const ObdTelemetry* telem, bool stale) {
    telem_ = telem;
    stale_ = stale;
}

void SettingsScreen::onEnter() {
    Serial.println("[UI] Settings");
}

void SettingsScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
}

}  // namespace ui
