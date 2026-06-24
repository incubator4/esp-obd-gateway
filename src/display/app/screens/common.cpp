#include "app/screens/common.h"

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
    footer_label_ = createFooter(root_, layout_, nav_, true);

    lv_obj_t* label = lv_label_create(root_);
    lv_label_set_text(label, "按 BOOT 切换界面");
    lv_obj_set_style_text_color(label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_center(label);
}

void SettingsScreen::onExit() {
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
}

void SettingsScreen::onTick(uint32_t now_ms) { (void)now_ms; }

}  // namespace ui
