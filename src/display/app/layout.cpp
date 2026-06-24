#include "app/layout.h"

#include <algorithm>

namespace ui {

namespace {

int16_t clampArcSize(uint16_t width, int16_t slot_h) {
    const int16_t by_width = static_cast<int16_t>(width) - 32;
    const int16_t by_height = slot_h - 36;
    return std::max<int16_t>(64, std::min(by_width, by_height));
}

GaugeSlot makeSlot(int16_t x, int16_t y, int16_t w, int16_t h) {
    GaugeSlot slot{};
    slot.x = x;
    slot.y = y;
    slot.w = w;
    slot.h = h;
    slot.arc_size = clampArcSize(static_cast<uint16_t>(w), h);
    return slot;
}

}  // namespace

PanelLayout PanelLayout::fromSize(uint16_t width, uint16_t height) {
    PanelLayout layout{};
    layout.width = width;
    layout.height = height;
    layout.header_h = 28;
    layout.footer_h = (height >= 280) ? 24 : 20;
    layout.content_y = layout.header_h;
    layout.content_h = height - layout.header_h - layout.footer_h;
    return layout;
}

GaugeSlot PanelLayout::singleGauge() const {
    return makeSlot(0, static_cast<int16_t>(content_y),
                    static_cast<int16_t>(width),
                    static_cast<int16_t>(content_h));
}

GaugeSlot PanelLayout::topHalf() const {
    const int16_t half = static_cast<int16_t>(content_h / 2);
    return makeSlot(0, static_cast<int16_t>(content_y),
                    static_cast<int16_t>(width), half);
}

GaugeSlot PanelLayout::bottomHalf() const {
    const int16_t half = static_cast<int16_t>(content_h / 2);
    return makeSlot(0, static_cast<int16_t>(content_y + half),
                    static_cast<int16_t>(width), half);
}

}  // namespace ui
