#pragma once

#include <cstddef>
#include <cstdint>

#include "app/screen.h"

namespace disp {
class Input;
}

namespace ui {

class Navigator {
public:
    static constexpr size_t kMaxScreens = 8;

    void bindInput(disp::Input* input) { input_ = input; }

    bool addScreen(Screen* screen);
    bool show(ScreenId id);
    void next();
    void prev();

    /** Restore last screen from NVS if registered; no-op if unset or invalid. */
    bool restoreSavedScreen();

    Screen* current() { return current_; }
    size_t count() const { return count_; }

    void tick(uint32_t now_ms = 0);
    void handleInput();

private:
    Screen* find(ScreenId id) const;
    int indexOf(Screen* screen) const;

    disp::Input* input_ = nullptr;
    Screen* screens_[kMaxScreens]{};
    size_t count_ = 0;
    Screen* current_ = nullptr;
};

}  // namespace ui
