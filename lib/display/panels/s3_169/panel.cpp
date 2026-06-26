#include "panels/s3_169/panel.h"

#if defined(DISPLAY_PROFILE_S3_169)

#include "config_link.h"

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <lvgl.h>

namespace disp {

namespace {

Arduino_DataBus* g_bus = nullptr;
Arduino_GFX* g_gfx = nullptr;

constexpr uint32_t kBootDebounceMs = 30;

void enableBoardPower() {
    pinMode(DISP_S3_SYS_EN, OUTPUT);
    digitalWrite(DISP_S3_SYS_EN, HIGH);
}

bool probeI2cDevice(uint8_t addr) {
    Wire.beginTransmission(addr);
    return Wire.endTransmission() == 0;
}

}  // namespace

bool S3169Panel::begin() {
    enableBoardPower();

    pinMode(DISP_S3_LCD_BL, OUTPUT);
    digitalWrite(DISP_S3_LCD_BL, HIGH);

    g_bus = new Arduino_ESP32SPI(DISP_S3_LCD_DC, DISP_S3_LCD_CS, DISP_S3_LCD_CLK,
                                 DISP_S3_LCD_MOSI);
    g_gfx = new Arduino_ST7789(g_bus, DISP_S3_LCD_RST, 0, true, DISP_S3_HOR_RES,
                               DISP_S3_VER_RES, 0, 20, 0, 0);

    if (g_gfx == nullptr || !g_gfx->begin()) {
        Serial.println("[S3-1.69] ST7789 init failed");
        return false;
    }

    g_gfx->fillScreen(RGB565_BLACK);
    setBacklight(80);
    running_ = true;
    Serial.println("[S3-1.69] ST7789 ready");
    return true;
}

PanelSize S3169Panel::size() const {
    return {DISP_S3_HOR_RES, DISP_S3_VER_RES};
}

void S3169Panel::setBacklight(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    pinMode(DISP_S3_LCD_BL, OUTPUT);
    if (percent == 0) {
        digitalWrite(DISP_S3_LCD_BL, LOW);
        return;
    }
    if (percent >= 100) {
        digitalWrite(DISP_S3_LCD_BL, HIGH);
        return;
    }
    static bool attached = false;
    if (!attached) {
        ledcAttach(DISP_S3_LCD_BL, 5000, 8);
        attached = true;
    }
    ledcWrite(DISP_S3_LCD_BL, static_cast<uint32_t>((percent * 255U) / 100U));
}

void S3169Panel::flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                           const uint16_t* rgb565) {
    if (!running_ || g_gfx == nullptr || rgb565 == nullptr) {
        return;
    }

    const int32_t w = x2 - x1 + 1;
    const int32_t h = y2 - y1 + 1;
    if (w <= 0 || h <= 0) {
        return;
    }

#if LV_COLOR_16_SWAP
    g_gfx->draw16bitBeRGBBitmap(x1, y1, const_cast<uint16_t*>(rgb565), w, h);
#else
    g_gfx->draw16bitRGBBitmap(x1, y1, const_cast<uint16_t*>(rgb565), w, h);
#endif
}

bool S3169Input::begin() {
    enableBoardPower();

    Wire.begin(DISP_S3_I2C_SDA, DISP_S3_I2C_SCL);
    Wire.setClock(400000);

    pinMode(DISP_S3_TP_RST, OUTPUT);
    digitalWrite(DISP_S3_TP_RST, LOW);
    delay(10);
    digitalWrite(DISP_S3_TP_RST, HIGH);
    delay(50);

    pinMode(DISP_S3_TP_INT, INPUT_PULLUP);
    pinMode(0, INPUT_PULLUP);
    pinMode(DISP_S3_SYS_OUT, INPUT_PULLUP);

    touch_ready_ = probeI2cDevice(DISP_S3_TP_ADDR);
    if (touch_ready_) {
        Serial.printf("[S3-1.69] CST816T touch detected (0x%02X)\n", DISP_S3_TP_ADDR);
        // TODO: read coordinates from CST816T
    } else {
        Serial.println("[S3-1.69] no touch controller — BOOT/PWR only");
    }

    boot_prev_ = digitalRead(0) == HIGH;
    boot_pressed_ = digitalRead(0) == LOW;
    boot_press_start_ms_ = millis();
    return true;
}

void S3169Input::poll() {
    const bool boot_now = digitalRead(0) == HIGH;
    const bool boot_down = !boot_now;
    const uint32_t now = millis();

    if (boot_down && boot_pressed_) {
        if (!boot_long_fired_ && (now - boot_press_start_ms_ >= LINK_GATEWAY_PAIR_HOLD_MS)) {
            boot_long_fired_ = true;
            boot_long_pending_ = true;
            boot_click_ = false;
        }
    }

    if (boot_prev_ && !boot_now) {
        boot_press_start_ms_ = now;
        boot_long_fired_ = false;
    } else if (!boot_prev_ && boot_now) {
        if (!boot_long_fired_) {
            boot_click_ = true;
        }
    }

    boot_prev_ = boot_now;
    boot_pressed_ = boot_down;

    if (!touch_ready_) {
        touch_active_ = false;
        return;
    }

    touch_active_ = digitalRead(DISP_S3_TP_INT) == LOW;
    if (touch_active_) {
        // TODO: read CST816T coordinates
        touch_x_ = 0;
        touch_y_ = 0;
    }
}

bool S3169Input::touchPoint(int16_t& x, int16_t& y) const {
    if (!touch_active_) {
        return false;
    }
    x = touch_x_;
    y = touch_y_;
    return true;
}

bool S3169Input::buttonDown(InputButton btn) const {
    switch (btn) {
        case InputButton::Boot:
            return digitalRead(0) == LOW;
        case InputButton::Power:
            return digitalRead(DISP_S3_SYS_OUT) == LOW;
        default:
            return false;
    }
}

bool S3169Input::buttonClicked(InputButton btn) {
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

bool S3169Input::buttonLongPressed(InputButton btn) {
    if (btn == InputButton::Boot && boot_long_pending_) {
        boot_long_pending_ = false;
        return true;
    }
    return false;
}

}  // namespace disp

#endif  // DISPLAY_PROFILE_S3_169
