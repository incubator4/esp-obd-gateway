#include "app/widgets/screen_chrome.h"

#include <cstdio>

namespace ui {

void styleScreenRoot(lv_obj_t* root) {
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
}

lv_obj_t* createHeader(lv_obj_t* parent, const char* title, uint16_t header_h) {
    lv_obj_t* header = lv_obj_create(parent);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, LV_PCT(100), header_h);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(header);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_color(label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_center(label);
    return header;
}

lv_obj_t* createFooter(lv_obj_t* parent, const PanelLayout& layout,
                       const ScreenNavInfo& nav, bool boot_only) {
    if (layout.footer_h == 0) {
        return nullptr;
    }

    lv_obj_t* footer = lv_obj_create(parent);
    lv_obj_remove_style_all(footer);
    lv_obj_set_size(footer, LV_PCT(100), layout.footer_h);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(footer, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(footer);
    lv_obj_set_style_text_color(label, lv_color_hex(0x666666), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(label);
    updateFooterStatus(label, nullptr, true, nav, boot_only);
    return label;
}

void updateFooterStatus(lv_obj_t* footer_label, const ObdTelemetry* telem, bool stale,
                        const ScreenNavInfo& nav, bool boot_only) {
    if (footer_label == nullptr) {
        return;
    }

    char buf[48];
    if (boot_only) {
        std::snprintf(buf, sizeof(buf), "%u/%u  BOOT>", nav.index, nav.total);
        lv_label_set_text(footer_label, buf);
        lv_obj_set_style_text_color(footer_label, lv_color_hex(0x888888), LV_PART_MAIN);
        return;
    }

    const char* obd = "OBD --";
    uint32_t obd_color = 0x666666;
    if (!stale && telem != nullptr) {
        const bool connected = (telem->flags & TELEM_FLAG_OBD_CONNECTED) != 0;
        const bool can_ok = (telem->flags & TELEM_FLAG_CAN_OK) != 0;
        if (connected && can_ok) {
            obd = "OBD OK";
            obd_color = 0x00D4AA;
        } else if (connected) {
            obd = "OBD";
            obd_color = 0xFFAA00;
        }
    }

    std::snprintf(buf, sizeof(buf), "%s  %u/%u  BOOT>", obd, nav.index, nav.total);
    lv_label_set_text(footer_label, buf);
    lv_obj_set_style_text_color(footer_label, lv_color_hex(obd_color), LV_PART_MAIN);
}

}  // namespace ui
