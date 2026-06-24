#include "app/widgets/value_gauge.h"

#include <algorithm>
#include <cstdio>

namespace ui {

namespace {

const lv_font_t* titleFont(bool compact) {
    return compact ? &lv_font_montserrat_14 : &lv_font_montserrat_20;
}

const lv_font_t* valueFont(bool compact) {
    return compact ? &lv_font_montserrat_20 : &lv_font_montserrat_28;
}

int32_t clampValue(int32_t value, int32_t min_v, int32_t max_v) {
    return std::max(min_v, std::min(value, max_v));
}

}  // namespace

void ValueGauge::create(lv_obj_t* parent, const GaugeSlot& slot, const Config& config) {
    destroy();
    config_ = config;

    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_pos(root_, slot.x, slot.y);
    lv_obj_set_size(root_, slot.w, slot.h);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    const int16_t arc_size = slot.arc_size;
    const int16_t arc_x = (slot.w - arc_size) / 2;
    const int16_t arc_y = config.compact ? 4 : 8;

    arc_ = lv_arc_create(root_);
    lv_obj_set_size(arc_, arc_size, arc_size);
    lv_obj_set_pos(arc_, arc_x, arc_y);
    lv_arc_set_rotation(arc_, 135);
    lv_arc_set_bg_angles(arc_, 0, 270);
    lv_arc_set_range(arc_, config.min_value, config.max_value);
    lv_arc_set_value(arc_, config.min_value);
    lv_obj_remove_style(arc_, nullptr, LV_PART_KNOB);
    lv_obj_clear_flag(arc_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_, config.compact ? 6 : 8, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, config.compact ? 6 : 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, lv_color_hex(config.arc_color), LV_PART_INDICATOR);

    lv_obj_t* title = lv_label_create(root_);
    lv_label_set_text(title, config.title);
    lv_obj_set_style_text_color(title, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, titleFont(config.compact), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, config.compact ? 0 : 2);

    value_label_ = lv_label_create(root_);
    lv_label_set_text(value_label_, "--");
    lv_obj_set_style_text_color(value_label_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(value_label_, valueFont(config.compact), LV_PART_MAIN);
    lv_obj_align_to(value_label_, arc_, LV_ALIGN_CENTER, 0, config.compact ? -4 : -6);

    unit_label_ = lv_label_create(root_);
    lv_label_set_text(unit_label_, config.unit);
    lv_obj_set_style_text_color(unit_label_, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_style_text_font(unit_label_, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align_to(unit_label_, value_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

void ValueGauge::destroy() {
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    arc_ = nullptr;
    value_label_ = nullptr;
    unit_label_ = nullptr;
    last_value_ = -1;
    last_stale_ = true;
}

void ValueGauge::setValue(int32_t value, bool stale) {
    if (value_label_ == nullptr || arc_ == nullptr) {
        return;
    }
    if (value == last_value_ && stale == last_stale_) {
        return;
    }
    last_value_ = value;
    last_stale_ = stale;
    refreshLabels(value, stale);
}

void ValueGauge::refreshLabels(int32_t value, bool stale) {
    if (stale) {
        lv_label_set_text(value_label_, "--");
        lv_arc_set_value(arc_, config_.min_value);
        lv_obj_set_style_arc_color(arc_, lv_color_hex(config_.stale_color), LV_PART_INDICATOR);
        lv_obj_set_style_text_color(value_label_, lv_color_hex(0x666666), LV_PART_MAIN);
        return;
    }

    const int32_t clamped = clampValue(value, config_.min_value, config_.max_value);
    lv_arc_set_value(arc_, clamped);
    lv_obj_set_style_arc_color(arc_, lv_color_hex(config_.arc_color), LV_PART_INDICATOR);
    lv_obj_set_style_text_color(value_label_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    char buf[16];
    std::snprintf(buf, sizeof(buf), "%ld", static_cast<long>(value));
    lv_label_set_text(value_label_, buf);
}

}  // namespace ui
