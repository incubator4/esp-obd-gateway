#pragma once

#include <cstddef>
#include <cstdint>

namespace ui {

/** 通用界面 ID；板级可扩展专用 Screen */
enum class ScreenId : uint8_t {
    Rpm = 0,
    Speed = 1,
    EngineLoad = 2,
    CoolantTemp = 3,
    FuelPressure = 4,
    IntakeMap = 5,
    TimingAdvance = 6,
    IntakeTemp = 7,
    Maf = 8,
    Throttle = 9,
    TurboPressure = 10,
    TurboRpm = 11,
    Settings = 12,
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
