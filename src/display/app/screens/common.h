#pragma once

#include "app/layout.h"
#include "app/screen.h"
#include "app/widgets/screen_chrome.h"

#include <lvgl.h>

namespace ui {

class SettingsScreen : public Screen {
public:
    void init(const PanelLayout& layout, const ScreenNavInfo& nav);

    ScreenId id() const override { return ScreenId::Settings; }
    const char* title() const override { return "Settings"; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;

private:
    void refresh(uint32_t now_ms);

    PanelLayout layout_{};
    ScreenNavInfo nav_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* footer_label_ = nullptr;
    lv_obj_t* status_label_ = nullptr;
    lv_obj_t* mac_label_ = nullptr;
    lv_obj_t* hint_label_ = nullptr;
};

}  // namespace ui
