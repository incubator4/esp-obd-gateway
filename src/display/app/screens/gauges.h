#pragma once

#include "app/layout.h"
#include "app/screen.h"
#include "app/telemetry.h"
#include "app/widgets/screen_chrome.h"
#include "app/widgets/value_gauge.h"
#include "protocol.h"

#include <lvgl.h>

namespace ui {

/** 发动机转速全屏界面 */
class RpmScreen : public Screen, public TelemetryConsumer {
public:
    void init(const PanelLayout& layout, const ScreenNavInfo& nav);

    ScreenId id() const override { return ScreenId::Rpm; }
    const char* title() const override { return "RPM"; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;
    void setTelemetry(const ObdTelemetry* telem, bool stale) override;

private:
    void refresh();

    PanelLayout layout_{};
    ScreenNavInfo nav_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* footer_label_ = nullptr;
    ValueGauge gauge_{};
    const ObdTelemetry* telem_ = nullptr;
    bool stale_ = true;
    bool dirty_ = false;
};

/** 当前时速全屏界面 */
class SpeedScreen : public Screen, public TelemetryConsumer {
public:
    void init(const PanelLayout& layout, const ScreenNavInfo& nav);

    ScreenId id() const override { return ScreenId::Speed; }
    const char* title() const override { return "Speed"; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;
    void setTelemetry(const ObdTelemetry* telem, bool stale) override;

private:
    void refresh();

    PanelLayout layout_{};
    ScreenNavInfo nav_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* footer_label_ = nullptr;
    ValueGauge gauge_{};
    const ObdTelemetry* telem_ = nullptr;
    bool stale_ = true;
    bool dirty_ = false;
};

}  // namespace ui
