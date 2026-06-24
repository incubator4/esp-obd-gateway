#include "app/screens/common.h"

#include "app/gateway_link.h"
#include "app/gateway_peer.h"

#include <cstdio>

namespace ui {

void SettingsScreen::init(const PanelLayout& layout, const ScreenNavInfo& nav) {
    layout_ = layout;
    nav_ = nav;
}

void SettingsScreen::onEnter() {
    root_ = lv_obj_create(nullptr);
    styleScreenRoot(root_);
    lv_screen_load(root_);

    createHeader(root_, "Settings", layout_.header_h);
    footer_label_ = createFooter(root_, layout_, nav_, false);

    const int16_t content_x = 12;
    const int16_t line_h = 22;
    int16_t y = static_cast<int16_t>(layout_.content_y + 8);

    lv_obj_t* gw_title = lv_label_create(root_);
    lv_label_set_text(gw_title, "Gateway");
    lv_obj_set_style_text_color(gw_title, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_font(gw_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_pos(gw_title, content_x, y);
    y = static_cast<int16_t>(y + line_h);

    mac_label_ = lv_label_create(root_);
    lv_obj_set_style_text_color(mac_label_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(mac_label_, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_pos(mac_label_, content_x, y);
    y = static_cast<int16_t>(y + line_h + 6);

    lv_obj_t* status_title = lv_label_create(root_);
    lv_label_set_text(status_title, "Link");
    lv_obj_set_style_text_color(status_title, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_pos(status_title, content_x, y);
    y = static_cast<int16_t>(y + line_h);

    status_label_ = lv_label_create(root_);
    lv_obj_set_style_text_font(status_label_, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_pos(status_label_, content_x, y);
    y = static_cast<int16_t>(y + line_h + 10);

    hint_label_ = lv_label_create(root_);
    lv_label_set_text(hint_label_, "Long press BOOT 2s to pair");
    lv_obj_set_style_text_color(hint_label_, lv_color_hex(0x666666), LV_PART_MAIN);
    lv_obj_set_style_text_font(hint_label_, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_pos(hint_label_, content_x, y);

    refresh(0);
}

void SettingsScreen::onExit() {
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
    status_label_ = nullptr;
    mac_label_ = nullptr;
    hint_label_ = nullptr;
}

void SettingsScreen::onTick(uint32_t now_ms) {
    refresh(now_ms);
}

void SettingsScreen::refresh(uint32_t now_ms) {
    if (root_ == nullptr) {
        return;
    }

    GatewayLink& link = gatewayLink();
    uint8_t mac[6]{};
    link.gatewayMac(mac);

    char mac_buf[20];
    gatewayMacFormat(mac_buf, sizeof(mac_buf), mac);
    lv_label_set_text(mac_label_, mac_buf);

    const char* status_text = "Not paired";
    uint32_t status_color = 0x666666;
    if (link.isPairing()) {
        status_text = "Pairing...";
        status_color = 0xFFAA00;
    } else if (link.hasGateway()) {
        status_text = "Paired";
        status_color = 0x00D4AA;
    }
    lv_label_set_text(status_label_, status_text);
    lv_obj_set_style_text_color(status_label_, lv_color_hex(status_color), LV_PART_MAIN);

    updateFooterStatus(footer_label_, nullptr, !link.hasGateway() || link.isPairing(), nav_,
                       false);

    (void)now_ms;
}

}  // namespace ui
