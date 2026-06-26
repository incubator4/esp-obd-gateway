#pragma once

#include <cstddef>
#include <cstdint>

#include "display_profiles.h"
#include "core/input.h"
#include "core/panel.h"

namespace disp {

/** Waveshare ESP32-C6-LCD/Touch-LCD-1.47：JD9853 + optional AXS5106L */
class C6147Panel : public Panel {
public:
    bool begin() override;
    PanelSize size() const override;
    void setBacklight(uint8_t percent) override;
    void flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                   const uint16_t* rgb565) override;

private:
    bool running_ = false;
};

class C6147Input : public Input {
public:
    bool begin() override;
    void poll() override;

    bool hasTouch() const override { return touch_ready_; }
    bool touchPoint(int16_t& x, int16_t& y) const override;

    bool buttonDown(InputButton btn) const override;
    bool buttonClicked(InputButton btn) override;
    bool buttonLongPressed(InputButton btn) override;
    bool swipeNext() override;
    bool swipePrev() override;

private:
    void pollTouch();
    bool readTouchFrame(uint8_t* data, size_t len);
    void handleHardwareGesture(uint8_t gesture);
    void onTouchRelease();

    bool touch_ready_ = false;
    bool touch_active_ = false;
    int16_t touch_x_ = 0;
    int16_t touch_y_ = 0;
    bool touch_pressed_ = false;
    int16_t touch_start_x_ = 0;
    int16_t touch_start_y_ = 0;
    int16_t touch_last_x_ = 0;
    int16_t touch_last_y_ = 0;
    uint32_t touch_press_ms_ = 0;
    uint32_t touch_int_low_ms_ = 0;
    uint8_t touch_release_frames_ = 0;
    bool swipe_next_pending_ = false;
    bool swipe_prev_pending_ = false;

    int boot_raw_prev_ = 1;
    bool boot_pressed_prev_ = false;
    bool boot_click_pending_ = false;
    bool boot_long_pending_ = false;
    bool boot_long_fired_ = false;
    uint32_t boot_press_start_ms_ = 0;
    uint32_t boot_last_edge_ms_ = 0;
};

}  // namespace disp
