#include "panels/c6_147/panel.h"

#if defined(DISPLAY_PROFILE_C6_147)

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
constexpr uint32_t kTouchIntDebounceMs = 2;
constexpr int16_t kSwipeMinHorizontalPx = 18;
constexpr uint8_t kAxs5106RegData = 0x01;
constexpr uint8_t kAxs5106FrameLen = 14;
constexpr uint8_t kAxs5106GestureSwipeLeft = 0x03;
constexpr uint8_t kAxs5106GestureSwipeRight = 0x04;
constexpr uint32_t kTouchI2cClockHz = 100000;

bool readAxs5106Register(uint8_t reg, uint8_t* data, size_t len) {
    Wire.beginTransmission(DISP_C6_TP_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(true) != 0) {
        return false;
    }
    delayMicroseconds(100);

    const int got =
        Wire.requestFrom(static_cast<uint8_t>(DISP_C6_TP_ADDR), static_cast<uint8_t>(len));
    if (got != static_cast<int>(len)) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        data[i] = Wire.read();
    }
    return true;
}

int16_t mapTouchX(uint16_t raw_x) {
    if (raw_x >= DISP_C6_HOR_RES) {
        raw_x = DISP_C6_HOR_RES - 1;
    }
    return static_cast<int16_t>((DISP_C6_HOR_RES - 1) - raw_x);
}

int16_t mapTouchY(uint16_t raw_y) {
    if (raw_y >= DISP_C6_VER_RES) {
        raw_y = DISP_C6_VER_RES - 1;
    }
    return static_cast<int16_t>(raw_y);
}

void initTouchI2c(uint32_t clock_hz) {
    Wire.end();
    delay(10);
    pinMode(DISP_C6_I2C_SDA, INPUT_PULLUP);
    pinMode(DISP_C6_I2C_SCL, INPUT_PULLUP);
    Wire.begin(DISP_C6_I2C_SDA, DISP_C6_I2C_SCL);
    Wire.setClock(clock_hz);
    Wire.setTimeout(100);
    delay(20);
}

void recoverTouchI2c(uint32_t clock_hz) {
    initTouchI2c(clock_hz);
}

bool probeTouchOnBus() {
    Wire.beginTransmission(DISP_C6_TP_ADDR);
    return Wire.endTransmission() == 0;
}

void hardwareResetAxs5106(uint8_t rst_pin) {
    pinMode(rst_pin, OUTPUT);
    digitalWrite(rst_pin, HIGH);
    delay(10);
    digitalWrite(rst_pin, LOW);
    delay(10);
    digitalWrite(rst_pin, HIGH);
    delay(50);
}

void deselectSdCard() {
    pinMode(DISP_C6_SD_CS, OUTPUT);
    digitalWrite(DISP_C6_SD_CS, HIGH);
}

void jd9853RegInit() {
    static const uint8_t init_operations[] = {
        BEGIN_WRITE,
        WRITE_COMMAND_8, 0x11,
        END_WRITE,
        DELAY, 120,

        BEGIN_WRITE,
        WRITE_C8_D16, 0xDF, 0x98, 0x53,
        WRITE_C8_D8, 0xB2, 0x23,

        WRITE_COMMAND_8, 0xB7,
        WRITE_BYTES, 4,
        0x00, 0x47, 0x00, 0x6F,

        WRITE_COMMAND_8, 0xBB,
        WRITE_BYTES, 6,
        0x1C, 0x1A, 0x55, 0x73, 0x63, 0xF0,

        WRITE_C8_D16, 0xC0, 0x44, 0xA4,
        WRITE_C8_D8, 0xC1, 0x16,

        WRITE_COMMAND_8, 0xC3,
        WRITE_BYTES, 8,
        0x7D, 0x07, 0x14, 0x06, 0xCF, 0x71, 0x72, 0x77,

        WRITE_COMMAND_8, 0xC4,
        WRITE_BYTES, 12,
        0x00, 0x00, 0xA0, 0x79, 0x0B, 0x0A, 0x16, 0x79, 0x0B, 0x0A, 0x16, 0x82,

        WRITE_COMMAND_8, 0xC8,
        WRITE_BYTES, 32,
        0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,
        0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,

        WRITE_COMMAND_8, 0xD0,
        WRITE_BYTES, 5,
        0x04, 0x06, 0x6B, 0x0F, 0x00,

        WRITE_C8_D16, 0xD7, 0x00, 0x30,
        WRITE_C8_D8, 0xE6, 0x14,
        WRITE_C8_D8, 0xDE, 0x01,

        WRITE_COMMAND_8, 0xB7,
        WRITE_BYTES, 5,
        0x03, 0x13, 0xEF, 0x35, 0x35,

        WRITE_COMMAND_8, 0xC1,
        WRITE_BYTES, 3,
        0x14, 0x15, 0xC0,

        WRITE_C8_D16, 0xC2, 0x06, 0x3A,
        WRITE_C8_D16, 0xC4, 0x72, 0x12,
        WRITE_C8_D8, 0xBE, 0x00,
        WRITE_C8_D8, 0xDE, 0x02,

        WRITE_COMMAND_8, 0xE5,
        WRITE_BYTES, 3,
        0x00, 0x02, 0x00,

        WRITE_COMMAND_8, 0xE5,
        WRITE_BYTES, 3,
        0x01, 0x02, 0x00,

        WRITE_C8_D8, 0xDE, 0x00,
        WRITE_C8_D8, 0x35, 0x00,
        WRITE_C8_D8, 0x3A, 0x05,

        WRITE_COMMAND_8, 0x2A,
        WRITE_BYTES, 4,
        0x00, 0x22, 0x00, 0xCD,

        WRITE_COMMAND_8, 0x2B,
        WRITE_BYTES, 4,
        0x00, 0x00, 0x01, 0x3F,

        WRITE_C8_D8, 0xDE, 0x02,

        WRITE_COMMAND_8, 0xE5,
        WRITE_BYTES, 3,
        0x00, 0x02, 0x00,

        WRITE_C8_D8, 0xDE, 0x00,
        WRITE_C8_D8, 0x36, 0x00,
        WRITE_COMMAND_8, 0x21,
        END_WRITE,

        DELAY, 10,

        BEGIN_WRITE,
        WRITE_COMMAND_8, 0x29,
        END_WRITE,
    };

    if (g_bus != nullptr) {
        g_bus->batchOperation(init_operations, sizeof(init_operations));
    }
}

}  // namespace

bool C6147Panel::begin() {
    deselectSdCard();

    pinMode(DISP_C6_LCD_BL, OUTPUT);
    digitalWrite(DISP_C6_LCD_BL, HIGH);

    g_bus = new Arduino_HWSPI(DISP_C6_LCD_DC, DISP_C6_LCD_CS, DISP_C6_LCD_SCLK,
                              DISP_C6_LCD_MOSI, DISP_C6_LCD_MISO);
    g_gfx = new Arduino_ST7789(g_bus, DISP_C6_LCD_RST, 0, false, DISP_C6_HOR_RES,
                               DISP_C6_VER_RES, DISP_C6_LCD_COL_OFFSET1, 0,
                               DISP_C6_LCD_COL_OFFSET2, 0);

    if (g_gfx == nullptr || !g_gfx->begin()) {
        Serial.println("[C6-1.47] display init failed");
        return false;
    }

    jd9853RegInit();
    g_gfx->setRotation(2);
    g_gfx->invertDisplay(true);
    g_gfx->fillScreen(RGB565_BLACK);
    setBacklight(80);
    running_ = true;
    Serial.println("[C6-1.47] JD9853 ready");
    return true;
}

PanelSize C6147Panel::size() const {
    return {DISP_C6_HOR_RES, DISP_C6_VER_RES};
}

void C6147Panel::setBacklight(uint8_t percent) {
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

void C6147Panel::flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
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

bool C6147Input::begin() {
    deselectSdCard();

    delay(50);
    initTouchI2c(kTouchI2cClockHz);
    hardwareResetAxs5106(DISP_C6_TP_RST);
    pinMode(DISP_C6_TP_INT, INPUT_PULLUP);

    touch_ready_ = probeTouchOnBus();
    if (touch_ready_) {
        Serial.printf("[C6-1.47] AXS5106L touch detected (0x%02X)\n", DISP_C6_TP_ADDR);
    } else {
        Serial.println("[C6-1.47] no touch controller — BOOT only");
    }

    touch_active_ = false;
    touch_pressed_ = false;
    touch_release_frames_ = 0;
    swipe_next_pending_ = false;
    swipe_prev_pending_ = false;
    touch_int_low_ms_ = 0;

    pinMode(DISP_C6_BOOT_PIN, INPUT_PULLUP);
    boot_raw_prev_ = digitalRead(DISP_C6_BOOT_PIN);
    boot_pressed_prev_ = boot_raw_prev_ == LOW;
    boot_click_pending_ = false;
    boot_long_pending_ = false;
    boot_long_fired_ = false;
    boot_press_start_ms_ = millis();
    boot_last_edge_ms_ = millis();
    return true;
}

bool C6147Input::readTouchFrame(uint8_t* data, size_t len) {
    if (!touch_ready_ || data == nullptr || len < kAxs5106FrameLen) {
        return false;
    }
    if (readAxs5106Register(kAxs5106RegData, data, kAxs5106FrameLen)) {
        return true;
    }
    recoverTouchI2c(kTouchI2cClockHz);
    return false;
}

void C6147Input::handleHardwareGesture(uint8_t gesture) {
    if (gesture == kAxs5106GestureSwipeLeft) {
        swipe_next_pending_ = true;
    } else if (gesture == kAxs5106GestureSwipeRight) {
        swipe_prev_pending_ = true;
    }
}

void C6147Input::onTouchRelease() {
    if (swipe_next_pending_ || swipe_prev_pending_) {
        return;
    }

    const int16_t dx = touch_last_x_ - touch_start_x_;
    const int16_t dy = touch_last_y_ - touch_start_y_;

    if (abs(dx) < kSwipeMinHorizontalPx) {
        return;
    }
    if (abs(dy) > abs(dx)) {
        return;
    }

    if (dx < 0) {
        swipe_next_pending_ = true;
    } else {
        swipe_prev_pending_ = true;
    }
}

void C6147Input::pollTouch() {
    if (!touch_ready_) {
        return;
    }

    const bool int_active = digitalRead(DISP_C6_TP_INT) == LOW;
    if (!int_active && !touch_pressed_) {
        touch_active_ = false;
        return;
    }

    const uint32_t now = millis();
    if (int_active && !touch_pressed_) {
        if (touch_int_low_ms_ == 0) {
            touch_int_low_ms_ = now;
        }
        if (now - touch_int_low_ms_ < kTouchIntDebounceMs) {
            return;
        }
    } else if (!int_active) {
        touch_int_low_ms_ = 0;
    }

    uint8_t frame[kAxs5106FrameLen]{};
    if (!readTouchFrame(frame, sizeof(frame))) {
        return;
    }

    const uint8_t point_count = frame[1] & 0x0F;
    if (point_count > 0) {
        const uint16_t raw_x =
            static_cast<uint16_t>(((frame[2] & 0x0F) << 8) | frame[3]);
        const uint16_t raw_y =
            static_cast<uint16_t>(((frame[4] & 0x0F) << 8) | frame[5]);
        if (raw_x != 0x0FFF && raw_y != 0x0FFF) {
            touch_release_frames_ = 0;
            touch_active_ = true;
            touch_x_ = mapTouchX(raw_x);
            touch_y_ = mapTouchY(raw_y);

            if (!touch_pressed_) {
                touch_pressed_ = true;
                touch_start_x_ = touch_x_;
                touch_start_y_ = touch_y_;
                touch_press_ms_ = now;
            }
            touch_last_x_ = touch_x_;
            touch_last_y_ = touch_y_;
            return;
        }
    }

    touch_active_ = false;
    if (!touch_pressed_) {
        return;
    }

    handleHardwareGesture(frame[0]);
    if (!swipe_next_pending_ && !swipe_prev_pending_) {
        onTouchRelease();
    }
    touch_pressed_ = false;
    touch_release_frames_ = 0;
}

void C6147Input::poll() {
    pollTouch();

    const int raw = digitalRead(DISP_C6_BOOT_PIN);
    const uint32_t now = millis();
    const bool pressed = raw == LOW;

    if (pressed && boot_pressed_prev_) {
        if (!boot_long_fired_ && (now - boot_press_start_ms_ >= LINK_GATEWAY_PAIR_HOLD_MS)) {
            boot_long_fired_ = true;
            boot_long_pending_ = true;
            boot_click_pending_ = false;
        }
    }

    if (raw != boot_raw_prev_) {
        if (now - boot_last_edge_ms_ >= kBootDebounceMs) {
            boot_raw_prev_ = raw;
            boot_last_edge_ms_ = now;

            if (pressed && !boot_pressed_prev_) {
                boot_press_start_ms_ = now;
                boot_long_fired_ = false;
            } else if (!pressed && boot_pressed_prev_) {
                if (!boot_long_fired_) {
                    boot_click_pending_ = true;
                }
            }
            boot_pressed_prev_ = pressed;
        }
    }
}

bool C6147Input::touchPoint(int16_t& x, int16_t& y) const {
    if (!touch_active_) {
        return false;
    }
    x = touch_x_;
    y = touch_y_;
    return true;
}

bool C6147Input::swipeNext() {
    if (!swipe_next_pending_) {
        return false;
    }
    swipe_next_pending_ = false;
    return true;
}

bool C6147Input::swipePrev() {
    if (!swipe_prev_pending_) {
        return false;
    }
    swipe_prev_pending_ = false;
    return true;
}

bool C6147Input::buttonDown(InputButton btn) const {
    if (btn == InputButton::Boot) {
        return digitalRead(DISP_C6_BOOT_PIN) == LOW;
    }
    return false;
}

bool C6147Input::buttonClicked(InputButton btn) {
    if (btn == InputButton::Boot && boot_click_pending_) {
        boot_click_pending_ = false;
        return true;
    }
    return false;
}

bool C6147Input::buttonLongPressed(InputButton btn) {
    if (btn == InputButton::Boot && boot_long_pending_) {
        boot_long_pending_ = false;
        return true;
    }
    return false;
}

}  // namespace disp

#endif  // DISPLAY_PROFILE_C6_147
