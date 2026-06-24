#pragma once

#include <cstdint>

namespace disp {

struct PanelSize {
    uint16_t width = 0;
    uint16_t height = 0;
};

/** 面板抽象：SPI/LVGL flush、背光、分辨率 */
class Panel {
public:
    virtual ~Panel() = default;

    virtual bool begin() = 0;
    virtual PanelSize size() const = 0;
    virtual void setBacklight(uint8_t percent) = 0;

    /** LVGL / 软件渲染 flush；暂为占位，后续接 Arduino_GFX / LVGL */
    virtual void flushArea(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                           const uint16_t* rgb565) = 0;
};

}  // namespace disp
