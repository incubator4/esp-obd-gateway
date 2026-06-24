#pragma once

#include "config_display_s3.h"
#include "core/input.h"
#include "core/panel.h"

namespace disp {

/** Waveshare ESP32-S3-Touch-LCD-1.69：240×280 + CST816T */
class S3Touch169Panel : public Panel {
public:
    bool begin() override;
    PanelSize size() const override;
    void setBacklight(uint8_t percent) override;
    void flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const uint16_t* rgb565) override;

private:
    bool running_ = false;
};

class S3Touch169Input : public Input {
public:
    bool begin() override;
    void poll() override;

    bool hasTouch() const override { return touch_active_; }
    bool touchPoint(int16_t& x, int16_t& y) const override;

    bool buttonDown(InputButton btn) const override;
    bool buttonClicked(InputButton btn) override;
    bool buttonLongPressed(InputButton btn) override;

private:
    bool touch_active_ = false;
    int16_t touch_x_ = 0;
    int16_t touch_y_ = 0;

    bool boot_prev_ = true;
    bool boot_pressed_ = false;
    bool boot_click_ = false;
    bool boot_long_pending_ = false;
    bool boot_long_fired_ = false;
    uint32_t boot_press_start_ms_ = 0;
    bool pwr_prev_ = true;
    bool pwr_click_ = false;
};

}  // namespace disp
