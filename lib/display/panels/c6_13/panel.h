#pragma once

#include "display_profiles.h"
#include "core/input.h"
#include "core/panel.h"

namespace disp {

/** Waveshare ESP32-C6-LCD-1.3：240×240 ST7789，BOOT 键 */
class C613Panel : public Panel {
public:
    bool begin() override;
    PanelSize size() const override;
    void setBacklight(uint8_t percent) override;
    void flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const uint16_t* rgb565) override;

private:
    bool running_ = false;
};

class C613Input : public Input {
public:
    bool begin() override;
    void poll() override;

    bool hasTouch() const override { return false; }
    bool touchPoint(int16_t& x, int16_t& y) const override;

    bool buttonDown(InputButton btn) const override;
    bool buttonClicked(InputButton btn) override;
    bool buttonLongPressed(InputButton btn) override;

private:
    int boot_raw_prev_ = 1;
    bool boot_pressed_prev_ = false;
    bool boot_click_pending_ = false;
    bool boot_long_pending_ = false;
    bool boot_long_fired_ = false;
    uint32_t boot_press_start_ms_ = 0;
    uint32_t boot_last_edge_ms_ = 0;
};

}  // namespace disp
