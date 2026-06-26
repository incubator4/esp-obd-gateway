#include "board/s3/screens/attitude.h"

#include "board/factory.h"
#include "features/imu_attitude.h"

#include <Arduino.h>
#include <cmath>
#include <cstdio>

namespace ui {

namespace {

constexpr uint32_t kSkyBlue = 0x2E7BD6;
constexpr uint32_t kGroundRed = 0x9B2D22;
constexpr uint32_t kBezelDark = 0x1A1A1A;
constexpr uint32_t kWingWhite = 0xF5F5F5;
constexpr uint32_t kCenterMark = 0xFFAA00;

lv_obj_t* makeSolidRect(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, lv_coord_t x,
                        lv_coord_t y, uint32_t color) {
    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    return obj;
}

void addPitchMark(lv_obj_t* parent, lv_coord_t ball_size, int8_t deg) {
    const lv_coord_t y =
        static_cast<lv_coord_t>(ball_size / 2 + (deg * ball_size) / 180 - 1);
    const lv_coord_t mark_w = static_cast<lv_coord_t>(ball_size / 6);
    lv_obj_t* mark = makeSolidRect(parent, mark_w, 2,
                                   static_cast<lv_coord_t>((ball_size - mark_w) / 2), y,
                                   kWingWhite);
    lv_obj_set_style_bg_opa(mark, LV_OPA_70, LV_PART_MAIN);
}

}  // namespace

void AttitudeHorizonScreen::init(const PanelLayout& layout, const ScreenNavInfo& nav) {
    layout_ = layout;
    nav_ = nav;
}

void AttitudeHorizonScreen::buildInstrument(int16_t x, int16_t y, int16_t w, int16_t h) {
    instrument_h_ = h;
    ball_size_ = static_cast<int16_t>(std::max(w, h) * 2.5f);

    horizon_panel_ = lv_obj_create(root_);
    lv_obj_remove_style_all(horizon_panel_);
    lv_obj_set_size(horizon_panel_, w, h);
    lv_obj_set_pos(horizon_panel_, x, y);
    lv_obj_set_style_bg_color(horizon_panel_, lv_color_hex(kBezelDark), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(horizon_panel_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(horizon_panel_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_border_width(horizon_panel_, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(horizon_panel_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(horizon_panel_, true, LV_PART_MAIN);
    lv_obj_clear_flag(horizon_panel_, LV_OBJ_FLAG_SCROLLABLE);

    horizon_ball_ = lv_obj_create(horizon_panel_);
    lv_obj_remove_style_all(horizon_ball_);
    lv_obj_set_size(horizon_ball_, ball_size_, ball_size_);
    lv_obj_align(horizon_ball_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(horizon_ball_, ball_size_ / 2, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(horizon_ball_, ball_size_ / 2, LV_PART_MAIN);
    lv_obj_clear_flag(horizon_ball_, LV_OBJ_FLAG_SCROLLABLE);

    makeSolidRect(horizon_ball_, ball_size_, ball_size_ / 2, 0, 0, kSkyBlue);
    makeSolidRect(horizon_ball_, ball_size_, ball_size_ / 2, 0, ball_size_ / 2, kGroundRed);

    horizon_line_ = makeSolidRect(horizon_ball_, ball_size_, 3, 0,
                                  static_cast<lv_coord_t>(ball_size_ / 2 - 1), kWingWhite);

    addPitchMark(horizon_ball_, ball_size_, -20);
    addPitchMark(horizon_ball_, ball_size_, -10);
    addPitchMark(horizon_ball_, ball_size_, 10);
    addPitchMark(horizon_ball_, ball_size_, 20);

    aircraft_wings_ = makeSolidRect(horizon_panel_, static_cast<lv_coord_t>(w * 55 / 100), 4,
                                    0, 0, kWingWhite);
    lv_obj_align(aircraft_wings_, LV_ALIGN_CENTER, 0, 0);

    aircraft_dot_ = lv_obj_create(horizon_panel_);
    lv_obj_remove_style_all(aircraft_dot_);
    lv_obj_set_size(aircraft_dot_, 8, 8);
    lv_obj_set_style_bg_color(aircraft_dot_, lv_color_hex(kCenterMark), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(aircraft_dot_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(aircraft_dot_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(aircraft_dot_, LV_ALIGN_CENTER, 0, 0);

    roll_pointer_ = lv_obj_create(horizon_panel_);
    lv_obj_remove_style_all(roll_pointer_);
    lv_obj_set_size(roll_pointer_, 14, 10);
    lv_obj_set_style_bg_color(roll_pointer_, lv_color_hex(kCenterMark), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roll_pointer_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_align(roll_pointer_, LV_ALIGN_TOP_MID, 0, 6);

    status_label_ = lv_label_create(horizon_panel_);
    lv_obj_set_style_text_color(status_label_, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label_, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(status_label_, LV_ALIGN_BOTTOM_MID, 0, -6);
}

void AttitudeHorizonScreen::onEnter() {
    root_ = lv_obj_create(nullptr);
    styleScreenRoot(root_);
    lv_screen_load(root_);

    createHeader(root_, "Attitude", layout_);
    footer_label_ = createFooter(root_, layout_, nav_, false);
    updateFooterStatus(footer_label_, nullptr, true, nav_, false);

    const int16_t content_x = static_cast<int16_t>(layout_.pad_x);
    const int16_t content_w =
        static_cast<int16_t>(layout_.width - layout_.pad_x * 2);
    const int16_t instrument_y =
        static_cast<int16_t>(layout_.content_y + layout_.pad_y / 2);
    const int16_t instrument_h =
        static_cast<int16_t>(layout_.content_h - layout_.pad_y);

    buildInstrument(content_x, instrument_y, content_w, instrument_h);

    Serial.println("[UI] Attitude horizon (IMU)");
    last_roll_ = 999.0f;
    last_pitch_ = 999.0f;
    onTick(0);
}

void AttitudeHorizonScreen::onExit() {
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
    horizon_panel_ = nullptr;
    horizon_ball_ = nullptr;
    horizon_line_ = nullptr;
    aircraft_wings_ = nullptr;
    aircraft_dot_ = nullptr;
    roll_pointer_ = nullptr;
    status_label_ = nullptr;
}

void AttitudeHorizonScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    const disp::ImuAttitudeFeature* imu = disp::imuAttitudeFeature();
    if (imu == nullptr) {
        refreshHorizon(0.0f, 0.0f, false, "IMU: N/A");
        return;
    }
    const disp::ImuAttitude& att = imu->attitude();
    refreshHorizon(att.roll_deg, att.pitch_deg, att.valid, imu->statusText());
}

void AttitudeHorizonScreen::onTouch(int16_t x, int16_t y) {
    (void)x;
    (void)y;
}

void AttitudeHorizonScreen::refreshHorizon(float roll_deg, float pitch_deg, bool valid,
                                            const char* status) {
    if (root_ == nullptr || horizon_ball_ == nullptr) {
        return;
    }

    const float roll_clamped = std::max(-60.0f, std::min(60.0f, roll_deg));
    const float pitch_clamped = std::max(-45.0f, std::min(45.0f, pitch_deg));

    if (std::fabs(roll_clamped - last_roll_) > 0.2f ||
        std::fabs(pitch_clamped - last_pitch_) > 0.2f) {
        last_roll_ = roll_clamped;
        last_pitch_ = pitch_clamped;

        const int16_t pitch_offset =
            static_cast<int16_t>((pitch_clamped / 90.0f) * (instrument_h_ / 2));
        lv_obj_align(horizon_ball_, LV_ALIGN_CENTER, 0, pitch_offset);
        lv_obj_set_style_transform_rotation(horizon_ball_,
                                            static_cast<int32_t>(-roll_clamped * 10),
                                            LV_PART_MAIN);
    }

    if (status_label_ != nullptr) {
        char buf[48];
        if (valid) {
            std::snprintf(buf, sizeof(buf), "R %+.0f  P %+.0f", roll_clamped, pitch_clamped);
        } else {
            std::snprintf(buf, sizeof(buf), "%s",
                          status != nullptr ? status : "IMU: not ready");
        }
        lv_label_set_text(status_label_, buf);
    }
}

}  // namespace ui
