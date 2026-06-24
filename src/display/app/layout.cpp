#include "app/layout.h"

#include <algorithm>

namespace ui {

namespace {

constexpr uint16_t kPermille = 1000;

uint16_t scaleU16(uint16_t ref, uint16_t permille) {
    return static_cast<uint16_t>((static_cast<uint32_t>(ref) * permille) / kPermille);
}

int16_t scaleI16(int16_t ref, uint16_t permille) {
    return static_cast<int16_t>((static_cast<int32_t>(ref) * permille) / kPermille);
}

int16_t clampArcSize(int16_t w, int16_t h) {
    const int16_t margin_h = scaleI16(w, 110);
    const int16_t by_width = w - margin_h * 2;
    const int16_t by_height = scaleI16(h, 720);
    const int16_t min_size = std::max<int16_t>(scaleI16(std::min(w, h), 260), 48);
    return std::max(min_size, std::min(by_width, by_height));
}

GaugeSlot makeSlot(int16_t x, int16_t y, int16_t w, int16_t h) {
    GaugeSlot slot{};
    slot.x = x;
    slot.y = y;
    slot.w = w;
    slot.h = h;
    slot.arc_size = clampArcSize(w, h);
    return slot;
}

}  // namespace

PanelLayout PanelLayout::fromSize(uint16_t width, uint16_t height) {
    PanelLayout layout{};
    layout.width = width;
    layout.height = height;
    layout.header_h = scaleU16(height, 115);
    layout.footer_h = scaleU16(height, 145);
    layout.pad_x = scaleU16(width, 50);
    layout.pad_y = scaleU16(height, 35);
    layout.line_h = scaleU16(height, 92);
    layout.arc_stroke = std::max<uint16_t>(scaleU16(std::min(width, height), 33), 6);
    layout.content_y = layout.header_h;
    layout.content_h = height - layout.header_h - layout.footer_h;
    return layout;
}

GaugeSlot PanelLayout::singleGauge() const {
    GaugeSlot slot =
        makeSlot(0, static_cast<int16_t>(content_y), static_cast<int16_t>(width),
                 static_cast<int16_t>(content_h));
    slot.arc_stroke = static_cast<int16_t>(arc_stroke);
    return slot;
}

GaugeSlot PanelLayout::topHalf() const {
    const int16_t half = static_cast<int16_t>(content_h / 2);
    GaugeSlot slot =
        makeSlot(0, static_cast<int16_t>(content_y), static_cast<int16_t>(width), half);
    slot.arc_stroke = static_cast<int16_t>(arc_stroke);
    return slot;
}

GaugeSlot PanelLayout::bottomHalf() const {
    const int16_t half = static_cast<int16_t>(content_h / 2);
    GaugeSlot slot =
        makeSlot(0, static_cast<int16_t>(content_y + half), static_cast<int16_t>(width), half);
    slot.arc_stroke = static_cast<int16_t>(arc_stroke);
    return slot;
}

}  // namespace ui
