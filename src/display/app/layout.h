#pragma once

#include <cstdint>

namespace ui {

/** 根据面板分辨率按比例计算的控件槽位 */
struct GaugeSlot {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;
    int16_t arc_size = 0;
    int16_t arc_stroke = 0;
};

/** 全屏 UI 响应式布局（所有尺寸由 width/height 比例推导） */
struct PanelLayout {
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t header_h = 0;
    uint16_t footer_h = 0;
    uint16_t content_y = 0;
    uint16_t content_h = 0;
    uint16_t pad_x = 0;
    uint16_t pad_y = 0;
    uint16_t line_h = 0;
    uint16_t arc_stroke = 0;

    static PanelLayout fromSize(uint16_t width, uint16_t height);

    GaugeSlot singleGauge() const;
    GaugeSlot topHalf() const;
    GaugeSlot bottomHalf() const;
};

}  // namespace ui
