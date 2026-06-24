#pragma once

#include <cstdint>

namespace ui {

/** 根据面板分辨率计算控件布局，240×240 与 240×280 共用逻辑 */
struct GaugeSlot {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;
    int16_t arc_size = 0;
};

struct PanelLayout {
    uint16_t width = 240;
    uint16_t height = 240;
    uint16_t header_h = 28;
    uint16_t footer_h = 16;
    uint16_t content_y = 28;
    uint16_t content_h = 212;

    static PanelLayout fromSize(uint16_t width, uint16_t height);

    GaugeSlot singleGauge() const;
    GaugeSlot topHalf() const;
    GaugeSlot bottomHalf() const;
};

}  // namespace ui
