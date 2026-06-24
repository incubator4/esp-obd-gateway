#include "panels/s3_touch_169/panel.h"

#include <Arduino.h>
#include <Wire.h>

namespace disp {

bool S3Touch169Panel::begin() {
    // TODO: ST7789 init (DISP_S3_LCD_*)
    pinMode(DISP_S3_LCD_BL, OUTPUT);
    setBacklight(80);
    running_ = true;
    return true;
}

PanelSize S3Touch169Panel::size() const {
    return {DISP_S3_HOR_RES, DISP_S3_VER_RES};
}

void S3Touch169Panel::setBacklight(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    const uint8_t level = static_cast<uint8_t>((percent * 255U) / 100U);
    analogWrite(DISP_S3_LCD_BL, level);
}

void S3Touch169Panel::flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                 const uint16_t* rgb565) {
    (void)x1;
    (void)y1;
    (void)x2;
    (void)y2;
    (void)rgb565;
    // TODO: blit to ST7789
}

bool S3Touch169Input::begin() {
    pinMode(DISP_S3_TP_RST, OUTPUT);
    digitalWrite(DISP_S3_TP_RST, LOW);
    delay(10);
    digitalWrite(DISP_S3_TP_RST, HIGH);
    delay(50);

    pinMode(DISP_S3_TP_INT, INPUT_PULLUP);
    pinMode(0, INPUT_PULLUP);
    pinMode(DISP_S3_SYS_OUT, INPUT_PULLUP);

    Wire.begin(DISP_S3_I2C_SDA, DISP_S3_I2C_SCL);
    // TODO: CST816T init at 0x15

    boot_prev_ = digitalRead(0) == HIGH;
    return true;
}

void S3Touch169Input::poll() {
    const bool boot_now = digitalRead(0) == HIGH;
    boot_click_ = boot_prev_ && !boot_now;
    boot_prev_ = boot_now;

    touch_active_ = digitalRead(DISP_S3_TP_INT) == LOW;
    if (touch_active_) {
        // TODO: read CST816T coordinates
        touch_x_ = 0;
        touch_y_ = 0;
    }
}

bool S3Touch169Input::touchPoint(int16_t& x, int16_t& y) const {
    if (!touch_active_) {
        return false;
    }
    x = touch_x_;
    y = touch_y_;
    return true;
}

bool S3Touch169Input::buttonDown(InputButton btn) const {
    switch (btn) {
        case InputButton::Boot:
            return digitalRead(0) == LOW;
        case InputButton::Power:
            return digitalRead(DISP_S3_SYS_OUT) == LOW;
        default:
            return false;
    }
}

bool S3Touch169Input::buttonClicked(InputButton btn) {
    if (btn == InputButton::Boot && boot_click_) {
        boot_click_ = false;
        return true;
    }
    if (btn == InputButton::Power && pwr_click_) {
        pwr_click_ = false;
        return true;
    }
    return false;
}

}  // namespace disp
