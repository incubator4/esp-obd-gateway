#pragma once

#include <cstdint>

namespace disp {

enum class InputButton : int8_t {
    Boot = 0,
    Reset = 1,
    Power = 2,
};

/** 输入抽象：电容触摸 / 物理按键 */
class Input {
public:
    virtual ~Input() = default;

    virtual bool begin() = 0;
    virtual void poll() = 0;

    virtual bool hasTouch() const = 0;
    virtual bool touchPoint(int16_t& x, int16_t& y) const = 0;

    virtual bool buttonDown(InputButton btn) const = 0;
    virtual bool buttonClicked(InputButton btn) = 0;
    virtual bool buttonLongPressed(InputButton btn) = 0;
};

}  // namespace disp
