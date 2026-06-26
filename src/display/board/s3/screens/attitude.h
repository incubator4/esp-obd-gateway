#pragma once

#include "app/layout.h"
#include "app/screen.h"
#include "app/widgets/screen_chrome.h"

#include <lvgl.h>

namespace ui {

/**
 * S3-Touch-LCD-1.69 专用：飞机姿态仪（红/蓝人工地平仪）。
 */
class AttitudeHorizonScreen : public Screen {
public:
    void init(const PanelLayout& layout, const ScreenNavInfo& nav);

    ScreenId id() const override { return ScreenId::AttitudeHorizon; }
    const char* title() const override { return "Attitude"; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;
    void onTouch(int16_t x, int16_t y) override;

private:
    void buildInstrument(int16_t x, int16_t y, int16_t w, int16_t h);
    void refreshHorizon(float roll_deg, float pitch_deg, bool valid, const char* status);

    PanelLayout layout_{};
    ScreenNavInfo nav_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* footer_label_ = nullptr;
    lv_obj_t* horizon_panel_ = nullptr;
    lv_obj_t* horizon_ball_ = nullptr;
    lv_obj_t* horizon_line_ = nullptr;
    lv_obj_t* aircraft_wings_ = nullptr;
    lv_obj_t* aircraft_dot_ = nullptr;
    lv_obj_t* roll_pointer_ = nullptr;
    lv_obj_t* status_label_ = nullptr;
    int16_t instrument_h_ = 0;
    int16_t ball_size_ = 0;
    float last_roll_ = 999.0f;
    float last_pitch_ = 999.0f;
};

}  // namespace ui
