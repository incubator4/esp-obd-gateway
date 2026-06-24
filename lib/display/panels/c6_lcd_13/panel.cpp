#include "panels/c6_lcd_13/panel.h"

#include <Arduino.h>

namespace disp {

bool C6Lcd13Panel::begin() {
    // TODO: Arduino_GFX / ST7789 init with DISP_C6_* pins
    pinMode(DISP_C6_LCD_BL, OUTPUT);
    setBacklight(80);
    running_ = true;
    return true;
}

PanelSize C6Lcd13Panel::size() const {
    return {DISP_C6_HOR_RES, DISP_C6_VER_RES};
}

void C6Lcd13Panel::setBacklight(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    const uint8_t level = static_cast<uint8_t>((percent * 255U) / 100U);
    analogWrite(DISP_C6_LCD_BL, level);
}

void C6Lcd13Panel::flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                              const uint16_t* rgb565) {
    (void)x1;
    (void)y1;
    (void)x2;
    (void)y2;
    (void)rgb565;
    // TODO: blit to ST7789
}

bool C6Lcd13Input::begin() {
    pinMode(0, INPUT_PULLUP);  // BOOT
    boot_prev_ = digitalRead(0) == HIGH;
    return true;
}

void C6Lcd13Input::poll() {
    const bool boot_now = digitalRead(0) == HIGH;
    boot_click_ = boot_prev_ && !boot_now;
    boot_prev_ = boot_now;
}

bool C6Lcd13Input::touchPoint(int16_t& x, int16_t& y) const {
    (void)x;
    (void)y;
    return false;
}

bool C6Lcd13Input::buttonDown(InputButton btn) const {
    if (btn == InputButton::Boot) {
        return digitalRead(0) == LOW;
    }
    return false;
}

bool C6Lcd13Input::buttonClicked(InputButton btn) {
    if (btn == InputButton::Boot && boot_click_) {
        boot_click_ = false;
        return true;
    }
    return false;
}

}  // namespace disp
