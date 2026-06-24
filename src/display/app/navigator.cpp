#include "app/navigator.h"
#include "app/screen.h"
#include "app/screen_store.h"
#include "core/input.h"

#include <Arduino.h>
#include <lvgl.h>

namespace ui {

bool Navigator::addScreen(Screen* screen) {
    if (screen == nullptr || count_ >= kMaxScreens) {
        return false;
    }
    for (size_t i = 0; i < count_; ++i) {
        if (screens_[i]->id() == screen->id()) {
            return false;
        }
    }
    screens_[count_++] = screen;
    if (current_ == nullptr) {
        current_ = screen;
        current_->onEnter();
    }
    return true;
}

Screen* Navigator::find(ScreenId id) const {
    for (size_t i = 0; i < count_; ++i) {
        if (screens_[i]->id() == id) {
            return screens_[i];
        }
    }
    return nullptr;
}

int Navigator::indexOf(Screen* screen) const {
    for (size_t i = 0; i < count_; ++i) {
        if (screens_[i] == screen) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool Navigator::show(ScreenId id) {
    Screen* next = find(id);
    if (next == nullptr || next == current_) {
        return next != nullptr;
    }
    if (current_ != nullptr) {
        current_->onExit();
    }
    current_ = next;
    current_->onEnter();
    lv_refr_now(nullptr);
    screenStoreSave(id);
    return true;
}

bool Navigator::restoreSavedScreen() {
    ScreenId saved = ScreenId::Rpm;
    if (!screenStoreLoad(saved)) {
        return false;
    }
    Screen* screen = find(saved);
    if (screen == nullptr) {
        Serial.printf("[NAV] saved screen id 0x%02X not registered, using default\n",
                      static_cast<unsigned>(static_cast<uint8_t>(saved)));
        return false;
    }
    if (current_ == screen) {
        return true;
    }
    Serial.printf("[NAV] restore screen %s\n", screen->title());
    if (current_ != nullptr) {
        current_->onExit();
    }
    current_ = screen;
    current_->onEnter();
    lv_refr_now(nullptr);
    return true;
}

void Navigator::next() {
    if (count_ == 0 || current_ == nullptr) {
#if defined(DISPLAY_DEBUG_BOOT)
        Serial.println("[NAV] next ignored (no screens)");
#endif
        return;
    }
    const int idx = indexOf(current_);
    if (idx < 0) {
#if defined(DISPLAY_DEBUG_BOOT)
        Serial.println("[NAV] next ignored (current not found)");
#endif
        return;
    }
    const size_t next_idx = static_cast<size_t>((idx + 1) % static_cast<int>(count_));
#if defined(DISPLAY_DEBUG_BOOT)
    Serial.printf("[NAV] next %d -> %u (%s)\n", idx, static_cast<unsigned>(next_idx),
                  screens_[next_idx]->title());
#endif
    show(screens_[next_idx]->id());
}

void Navigator::prev() {
    if (count_ == 0 || current_ == nullptr) {
        return;
    }
    const int idx = indexOf(current_);
    if (idx < 0) {
        return;
    }
    const int prev_idx = (idx - 1 + static_cast<int>(count_)) % static_cast<int>(count_);
    show(screens_[prev_idx]->id());
}

void Navigator::tick(uint32_t now_ms) {
    if (now_ms == 0) {
        now_ms = millis();
    }
    if (current_ != nullptr) {
        current_->onTick(now_ms);
    }
}

void Navigator::handleInput() {
    if (input_ == nullptr) {
        return;
    }

    input_->poll();

    if (input_->buttonClicked(disp::InputButton::Boot)) {
#if defined(DISPLAY_DEBUG_BOOT)
        Serial.printf("[NAV] BOOT -> next (current=%s)\n",
                      current_ != nullptr ? current_->title() : "null");
#endif
        next();
        if (current_ != nullptr) {
            current_->onButton(static_cast<int>(disp::InputButton::Boot));
        }
        return;
    }

    if (input_->buttonClicked(disp::InputButton::Power)) {
        prev();
        if (current_ != nullptr) {
            current_->onButton(static_cast<int>(disp::InputButton::Power));
        }
        return;
    }

    if (input_->hasTouch()) {
        int16_t x = 0;
        int16_t y = 0;
        if (input_->touchPoint(x, y) && current_ != nullptr) {
            current_->onTouch(x, y);
        }
    }
}

}  // namespace ui
