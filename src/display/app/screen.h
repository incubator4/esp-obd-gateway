#pragma once

#include <cstddef>
#include <cstdint>

namespace ui {

/** 通用界面 ID；板级可扩展专用 Screen */
enum class ScreenId : uint8_t {
    ObdDashboard = 0,
    Settings = 1,
    // S3 专用
    AttitudeHorizon = 0x80,
};

class Screen {
public:
    virtual ~Screen() = default;

    virtual ScreenId id() const = 0;
    virtual const char* title() const = 0;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onTick(uint32_t now_ms) {}
    virtual void onTouch(int16_t x, int16_t y) {}
    virtual void onButton(int button_id) {}
};

}  // namespace ui
