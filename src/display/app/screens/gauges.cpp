#include "app/screens/gauges.h"

namespace ui {

void RpmScreen::init(const PanelLayout& layout, const ScreenNavInfo& nav) {
    layout_ = layout;
    nav_ = nav;
}

void RpmScreen::onEnter() {
    root_ = lv_obj_create(nullptr);
    styleScreenRoot(root_);
    lv_screen_load(root_);

    createHeader(root_, "转速 RPM", layout_.header_h);
    footer_label_ = createFooter(root_, layout_, nav_, false);

    ValueGauge::Config cfg{};
    cfg.title = "RPM";
    cfg.unit = "rpm";
    cfg.min_value = 0;
    cfg.max_value = 8000;
    cfg.arc_color = 0xFF6B35;
    gauge_.create(root_, layout_.singleGauge(), cfg);
    refresh();
}

void RpmScreen::onExit() {
    gauge_.destroy();
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
}

void RpmScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    if (dirty_) {
        refresh();
    }
}

void RpmScreen::setTelemetry(const ObdTelemetry* telem, bool stale) {
    telem_ = telem;
    stale_ = stale;
    dirty_ = true;
}

void RpmScreen::refresh() {
    dirty_ = false;
    if (root_ == nullptr) {
        return;
    }

    updateFooterStatus(footer_label_, telem_, stale_, nav_, false);
    if (stale_ || telem_ == nullptr) {
        gauge_.setValue(0, true);
        return;
    }
    gauge_.setValue(static_cast<int32_t>(telem_->rpm), false);
}

void SpeedScreen::init(const PanelLayout& layout, const ScreenNavInfo& nav) {
    layout_ = layout;
    nav_ = nav;
}

void SpeedScreen::onEnter() {
    root_ = lv_obj_create(nullptr);
    styleScreenRoot(root_);
    lv_screen_load(root_);

    createHeader(root_, "时速", layout_.header_h);
    footer_label_ = createFooter(root_, layout_, nav_, false);

    ValueGauge::Config cfg{};
    cfg.title = "SPEED";
    cfg.unit = "km/h";
    cfg.min_value = 0;
    cfg.max_value = 240;
    cfg.arc_color = 0x00D4AA;
    gauge_.create(root_, layout_.singleGauge(), cfg);
    refresh();
}

void SpeedScreen::onExit() {
    gauge_.destroy();
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
}

void SpeedScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    if (dirty_) {
        refresh();
    }
}

void SpeedScreen::setTelemetry(const ObdTelemetry* telem, bool stale) {
    telem_ = telem;
    stale_ = stale;
    dirty_ = true;
}

void SpeedScreen::refresh() {
    dirty_ = false;
    if (root_ == nullptr) {
        return;
    }

    updateFooterStatus(footer_label_, telem_, stale_, nav_, false);
    if (stale_ || telem_ == nullptr) {
        gauge_.setValue(0, true);
        return;
    }
    gauge_.setValue(static_cast<int32_t>(telem_->speed_kmh), false);
}

}  // namespace ui
