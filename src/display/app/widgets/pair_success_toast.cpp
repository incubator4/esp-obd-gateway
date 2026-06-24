#include "app/widgets/pair_success_toast.h"

#include "app/gateway_peer.h"
#include "config_link.h"

#include <Arduino.h>

#include <cstdio>

namespace ui {

namespace {

constexpr uint32_t kFadeOutMs = 280;

}  // namespace

void PairSuccessToast::animArcCb(void* obj, int32_t value) {
    lv_arc_set_value(static_cast<lv_obj_t*>(obj), value);
}

void PairSuccessToast::animOpaCb(void* obj, int32_t value) {
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(obj), static_cast<lv_opa_t>(value), LV_PART_MAIN);
}

void PairSuccessToast::show(const uint8_t gateway_mac[6]) {
    dismiss();

    overlay_ = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(overlay_);
    lv_obj_set_size(overlay_, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(overlay_, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(overlay_, LV_OPA_60, LV_PART_MAIN);
    lv_obj_clear_flag(overlay_, LV_OBJ_FLAG_SCROLLABLE);

    card_ = lv_obj_create(overlay_);
    lv_obj_remove_style_all(card_);
    lv_obj_set_size(card_, 176, 132);
    lv_obj_center(card_);
    lv_obj_set_style_bg_color(card_, lv_color_hex(0x161616), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(card_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(card_, 10, LV_PART_MAIN);
    lv_obj_set_style_border_color(card_, lv_color_hex(0x00D4AA), LV_PART_MAIN);
    lv_obj_set_style_border_width(card_, 2, LV_PART_MAIN);
    lv_obj_set_style_opa(card_, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(card_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* ring = lv_arc_create(card_);
    lv_obj_set_size(ring, 52, 52);
    lv_obj_align(ring, LV_ALIGN_TOP_MID, 0, 10);
    lv_arc_set_rotation(ring, 270);
    lv_arc_set_bg_angles(ring, 0, 360);
    lv_arc_set_range(ring, 0, 360);
    lv_arc_set_value(ring, 0);
    lv_obj_remove_style(ring, nullptr, LV_PART_KNOB);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(ring, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ring, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ring, lv_color_hex(0x00D4AA), LV_PART_INDICATOR);

    lv_obj_t* icon = lv_label_create(card_);
    lv_label_set_text(icon, LV_SYMBOL_OK);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x00D4AA), LV_PART_MAIN);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_align_to(icon, ring, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* title = lv_label_create(card_);
    lv_label_set_text(title, "Paired!");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 68);

    char mac_buf[20];
    gatewayMacFormat(mac_buf, sizeof(mac_buf), gateway_mac);
    lv_obj_t* mac_label = lv_label_create(card_);
    lv_label_set_text(mac_label, mac_buf);
    lv_obj_set_style_text_color(mac_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_style_text_font(mac_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(mac_label, LV_ALIGN_TOP_MID, 0, 94);

    lv_anim_t arc_anim;
    lv_anim_init(&arc_anim);
    lv_anim_set_var(&arc_anim, ring);
    lv_anim_set_values(&arc_anim, 0, 360);
    lv_anim_set_duration(&arc_anim, 450);
    lv_anim_set_exec_cb(&arc_anim, animArcCb);
    lv_anim_set_path_cb(&arc_anim, lv_anim_path_ease_out);
    lv_anim_start(&arc_anim);

    lv_anim_t card_anim;
    lv_anim_init(&card_anim);
    lv_anim_set_var(&card_anim, card_);
    lv_anim_set_values(&card_anim, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&card_anim, 300);
    lv_anim_set_exec_cb(&card_anim, animOpaCb);
    lv_anim_start(&card_anim);

    shown_at_ms_ = millis();
}

void PairSuccessToast::tick(uint32_t now_ms) {
    if (overlay_ == nullptr) {
        return;
    }
    if (now_ms == 0) {
        now_ms = millis();
    }

    const uint32_t elapsed = now_ms - shown_at_ms_;
    if (elapsed < LINK_PAIR_SUCCESS_TOAST_MS) {
        return;
    }

    if (elapsed < LINK_PAIR_SUCCESS_TOAST_MS + kFadeOutMs) {
        const uint32_t fade_elapsed = elapsed - LINK_PAIR_SUCCESS_TOAST_MS;
        const lv_opa_t opa =
            static_cast<lv_opa_t>(LV_OPA_COVER -
                                  (fade_elapsed * LV_OPA_COVER) / kFadeOutMs);
        lv_obj_set_style_opa(overlay_, opa, LV_PART_MAIN);
        return;
    }

    dismiss();
}

void PairSuccessToast::dismiss() {
    if (overlay_ != nullptr) {
        lv_obj_del(overlay_);
        overlay_ = nullptr;
        card_ = nullptr;
    }
    shown_at_ms_ = 0;
}

PairSuccessToast& pairSuccessToast() {
    static PairSuccessToast toast;
    return toast;
}

}  // namespace ui
