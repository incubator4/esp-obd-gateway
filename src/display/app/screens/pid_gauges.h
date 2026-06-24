#pragma once

#include "app/layout.h"
#include "app/screen.h"
#include "app/telemetry.h"
#include "app/widgets/screen_chrome.h"
#include "app/widgets/value_gauge.h"
#include "protocol.h"

#include <lvgl.h>

namespace ui {

/** 绑定 ObdTelemetry 字段的通用全屏仪表界面 */
class TelemGaugeScreen : public Screen, public TelemetryConsumer {
public:
    enum class Field : uint8_t {
        EngineLoad,
        CoolantTemp,
        FuelPressure,
        IntakeMap,
        TimingAdvance,
        IntakeTemp,
        Maf,
        Throttle,
        TurboPressure,
        TurboRpm,
    };

    struct Init {
        ScreenId screen_id;
        const char* header_title;
        ValueGauge::Config gauge;
        Field field;
        uint16_t valid_bit;
    };

    void init(const PanelLayout& layout, const ScreenNavInfo& nav, const Init& cfg);

    ScreenId id() const override { return cfg_.screen_id; }
    const char* title() const override { return cfg_.header_title; }

    void onEnter() override;
    void onExit() override;
    void onTick(uint32_t now_ms) override;
    void setTelemetry(const ObdTelemetry* telem, bool stale) override;

private:
    void refresh();
    bool fieldValid(const ObdTelemetry* telem) const;
    int32_t fieldValue(const ObdTelemetry* telem) const;

    Init cfg_{};
    PanelLayout layout_{};
    ScreenNavInfo nav_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* footer_label_ = nullptr;
    ValueGauge gauge_{};
    const ObdTelemetry* telem_ = nullptr;
    bool stale_ = true;
    bool dirty_ = false;
};

class Navigator;
class TelemetryRegistry;

/** 注册 10 个 PID 仪表屏，返回已注册数量 */
size_t registerPidGaugeScreens(Navigator& nav, TelemetryRegistry& telem,
                               const PanelLayout& layout, uint8_t start_page,
                               uint8_t total_pages);

}  // namespace ui
