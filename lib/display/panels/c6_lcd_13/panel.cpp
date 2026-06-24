#include "panels/c6_lcd_13/panel.h"

#include "config_link.h"

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

namespace disp {

namespace {

Arduino_DataBus* g_bus = nullptr;
Arduino_GFX* g_gfx = nullptr;

constexpr uint32_t kBootDebounceMs = 30;

#if defined(DISPLAY_DEBUG_BOOT)
void logPinLevel(uint8_t pin, const char* label) {
    const int level = digitalRead(pin);
    Serial.printf("[BOOT] %s GPIO%d = %s\n", label, pin,
                  level == LOW ? "LOW" : "HIGH");
}
#endif

}  // namespace

bool C6Lcd13Panel::begin() {
    pinMode(DISP_C6_LCD_BL, OUTPUT);
    digitalWrite(DISP_C6_LCD_BL, HIGH);

    g_bus = new Arduino_HWSPI(DISP_C6_LCD_DC, DISP_C6_LCD_CS, DISP_C6_LCD_SCLK,
                              DISP_C6_LCD_MOSI);
    g_gfx = new Arduino_ST7789(g_bus, DISP_C6_LCD_RST, 0, true, DISP_C6_HOR_RES,
                               DISP_C6_VER_RES);

    if (g_gfx == nullptr || !g_gfx->begin()) {
        Serial.println("[C6] ST7789 init failed");
        return false;
    }

    g_gfx->fillScreen(RGB565_BLACK);
    setBacklight(80);
    running_ = true;
    Serial.println("[C6] ST7789 ready");
    return true;
}

PanelSize C6Lcd13Panel::size() const {
    return {DISP_C6_HOR_RES, DISP_C6_VER_RES};
}

void C6Lcd13Panel::setBacklight(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    pinMode(DISP_C6_LCD_BL, OUTPUT);
    if (percent == 0) {
        digitalWrite(DISP_C6_LCD_BL, LOW);
        return;
    }
    if (percent >= 100) {
        digitalWrite(DISP_C6_LCD_BL, HIGH);
        return;
    }
    static bool attached = false;
    if (!attached) {
        ledcAttach(DISP_C6_LCD_BL, 5000, 8);
        attached = true;
    }
    ledcWrite(DISP_C6_LCD_BL, static_cast<uint32_t>((percent * 255U) / 100U));
}

void C6Lcd13Panel::flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
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

bool C6Lcd13Input::begin() {
    pinMode(DISP_C6_BOOT_PIN, INPUT_PULLUP);
    boot_raw_prev_ = digitalRead(DISP_C6_BOOT_PIN);
    boot_pressed_prev_ = boot_raw_prev_ == LOW;
    boot_click_pending_ = false;
    boot_long_pending_ = false;
    boot_long_fired_ = false;
    boot_press_start_ms_ = millis();
    boot_last_edge_ms_ = millis();

#if defined(DISPLAY_DEBUG_BOOT)
    Serial.printf("[BOOT] using GPIO%d (ESP32-C6 board BOOT, active LOW)\n", DISP_C6_BOOT_PIN);
    logPinLevel(DISP_C6_BOOT_PIN, "init");
    pinMode(0, INPUT_PULLUP);
    logPinLevel(0, "probe GPIO0");
#endif
    return true;
}

void C6Lcd13Input::poll() {
    const int raw = digitalRead(DISP_C6_BOOT_PIN);
    const uint32_t now = millis();
    const bool pressed = raw == LOW;

    if (pressed && boot_pressed_prev_) {
        if (!boot_long_fired_ && (now - boot_press_start_ms_ >= LINK_GATEWAY_PAIR_HOLD_MS)) {
            boot_long_fired_ = true;
            boot_long_pending_ = true;
            boot_click_pending_ = false;
#if defined(DISPLAY_DEBUG_BOOT)
            Serial.println("[BOOT] long press latched -> pairing");
#endif
        }
    }

    if (raw != boot_raw_prev_) {
        if (now - boot_last_edge_ms_ >= kBootDebounceMs) {
#if defined(DISPLAY_DEBUG_BOOT)
            Serial.printf("[BOOT] edge GPIO%d %s -> %s\n", DISP_C6_BOOT_PIN,
                          boot_raw_prev_ == LOW ? "LOW" : "HIGH",
                          raw == LOW ? "LOW" : "HIGH");
#endif
            boot_raw_prev_ = raw;
            boot_last_edge_ms_ = now;

            if (pressed && !boot_pressed_prev_) {
                boot_press_start_ms_ = now;
                boot_long_fired_ = false;
#if defined(DISPLAY_DEBUG_BOOT)
                Serial.println("[BOOT] press");
#endif
            } else if (!pressed && boot_pressed_prev_) {
                if (!boot_long_fired_) {
                    boot_click_pending_ = true;
#if defined(DISPLAY_DEBUG_BOOT)
                    Serial.println("[BOOT] short release -> click");
#endif
                }
#if defined(DISPLAY_DEBUG_BOOT)
                else {
                    Serial.println("[BOOT] long release");
                }
#endif
            }
            boot_pressed_prev_ = pressed;
        }
    }
}

bool C6Lcd13Input::touchPoint(int16_t& x, int16_t& y) const {
    (void)x;
    (void)y;
    return false;
}

bool C6Lcd13Input::buttonDown(InputButton btn) const {
    if (btn == InputButton::Boot) {
        return digitalRead(DISP_C6_BOOT_PIN) == LOW;
    }
    return false;
}

bool C6Lcd13Input::buttonClicked(InputButton btn) {
    if (btn == InputButton::Boot && boot_click_pending_) {
        boot_click_pending_ = false;
#if defined(DISPLAY_DEBUG_BOOT)
        Serial.println("[BOOT] click consumed -> navigator");
#endif
        return true;
    }
    return false;
}

bool C6Lcd13Input::buttonLongPressed(InputButton btn) {
    if (btn == InputButton::Boot && boot_long_pending_) {
        boot_long_pending_ = false;
        return true;
    }
    return false;
}

}  // namespace disp
