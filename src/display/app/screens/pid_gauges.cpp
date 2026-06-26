#include "app/screens/pid_gauges.h"

#include "app/navigator.h"
#include "app/telemetry.h"

namespace ui {

namespace {

constexpr size_t kPidGaugeCount = 10;

TelemGaugeScreen g_pid_gauges[kPidGaugeCount];

struct PidGaugeDef {
    ScreenId screen_id;
    const char* header_title;
    const char* gauge_title;
    const char* gauge_unit;
    int32_t min_value;
    int32_t max_value;
    uint32_t arc_color;
    uint32_t stale_color;
    bool compact;
    uint16_t value_scale;
    bool show_decimal;
    TelemGaugeScreen::Field field;
    uint16_t valid_bit;
};

constexpr PidGaugeDef kPidGaugeDefs[kPidGaugeCount] = {
    {ScreenId::EngineLoad, "Engine Load", "LOAD", "%", 0, 100, 0xFF6B35, 0x555555, false, 1,
     false, TelemGaugeScreen::Field::EngineLoad, TELEM_VALID_ENGINE_LOAD},
    {ScreenId::CoolantTemp, "Coolant Temp", "COOLANT", "C", -40, 120, 0x00A8FF, 0x555555, false,
     1, false, TelemGaugeScreen::Field::CoolantTemp, TELEM_VALID_COOLANT},
    {ScreenId::FuelPressure, "Fuel Pressure", "FUEL", "kPa", 0, 765, 0xFFD166, 0x555555, false, 1,
     false, TelemGaugeScreen::Field::FuelPressure, TELEM_VALID_FUEL_PRESSURE},
    {ScreenId::IntakeMap, "Intake Pressure", "MAP", "kPa", 0, 255, 0x9B5DE5, 0x555555, false, 1,
     false, TelemGaugeScreen::Field::IntakeMap, TELEM_VALID_INTAKE_MAP},
    {ScreenId::TimingAdvance, "Ignition Advance", "TIMING", "deg", -64, 64, 0xF15BB5, 0x555555,
     false, 2, true, TelemGaugeScreen::Field::TimingAdvance, TELEM_VALID_TIMING_ADVANCE},
    {ScreenId::IntakeTemp, "Intake Temp", "IAT", "C", -40, 80, 0x00D4AA, 0x555555, false, 1, false,
     TelemGaugeScreen::Field::IntakeTemp, TELEM_VALID_INTAKE_TEMP},
    {ScreenId::Maf, "Air Flow", "MAF", "g/s", 0, 200, 0x06D6A0, 0x555555, false, 10, true,
     TelemGaugeScreen::Field::Maf, TELEM_VALID_MAF},
    {ScreenId::Throttle, "Throttle", "TPS", "%", 0, 100, 0xEF476F, 0x555555, false, 1, false,
     TelemGaugeScreen::Field::Throttle, TELEM_VALID_THROTTLE},
    {ScreenId::TurboPressure, "Turbo Pressure", "BOOST", "kPa", 0, 300, 0x118AB2, 0x555555, false,
     1, false, TelemGaugeScreen::Field::TurboPressure, TELEM_VALID_TURBO_PRESSURE},
    {ScreenId::TurboRpm, "Turbo RPM", "T-RPM", "rpm", 0, 200000, 0x073B4C, 0x555555, false, 1,
     false, TelemGaugeScreen::Field::TurboRpm, TELEM_VALID_TURBO_RPM},
};

ValueGauge::Config makeGaugeConfig(const PidGaugeDef& def) {
    ValueGauge::Config cfg;
    cfg.title = def.gauge_title;
    cfg.unit = def.gauge_unit;
    cfg.min_value = def.min_value;
    cfg.max_value = def.max_value;
    cfg.arc_color = def.arc_color;
    cfg.stale_color = def.stale_color;
    cfg.compact = def.compact;
    cfg.value_scale = def.value_scale;
    cfg.show_decimal = def.show_decimal;
    return cfg;
}

TelemGaugeScreen::Init makeScreenInit(const PidGaugeDef& def) {
    TelemGaugeScreen::Init init;
    init.screen_id = def.screen_id;
    init.header_title = def.header_title;
    init.gauge = makeGaugeConfig(def);
    init.field = def.field;
    init.valid_bit = def.valid_bit;
    return init;
}

}  // namespace

void TelemGaugeScreen::init(const PanelLayout& layout, const ScreenNavInfo& nav,
                            const Init& cfg) {
    layout_ = layout;
    nav_ = nav;
    cfg_ = cfg;
}

void TelemGaugeScreen::onEnter() {
    root_ = lv_obj_create(nullptr);
    styleScreenRoot(root_);
    lv_screen_load(root_);

    createHeader(root_, cfg_.header_title, layout_);
    footer_label_ = createFooter(root_, layout_, nav_, false);
    gauge_.create(root_, layout_.singleGauge(), cfg_.gauge);
    refresh();
}

void TelemGaugeScreen::onExit() {
    gauge_.destroy();
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
    footer_label_ = nullptr;
}

void TelemGaugeScreen::onTick(uint32_t now_ms) {
    (void)now_ms;
    if (dirty_) {
        refresh();
    }
}

void TelemGaugeScreen::setTelemetry(const ObdTelemetry* telem, bool stale) {
    telem_ = telem;
    stale_ = stale;
    dirty_ = true;
}

bool TelemGaugeScreen::fieldValid(const ObdTelemetry* telem) const {
    if (telem == nullptr) {
        return false;
    }
    return (telem->valid_mask & cfg_.valid_bit) != 0;
}

int32_t TelemGaugeScreen::fieldValue(const ObdTelemetry* telem) const {
    switch (cfg_.field) {
        case Field::EngineLoad:
            return telem->engine_load_pct;
        case Field::CoolantTemp:
            return telem->coolant_c;
        case Field::FuelPressure:
            return static_cast<int32_t>(telem->fuel_pressure_kpa);
        case Field::IntakeMap:
            return telem->intake_map_kpa;
        case Field::TimingAdvance:
            return telem->timing_advance_deg_x2;
        case Field::IntakeTemp:
            return telem->intake_temp_c;
        case Field::Maf:
            return static_cast<int32_t>(telem->maf_gps_x10);
        case Field::Throttle:
            return telem->throttle_pct;
        case Field::TurboPressure:
            return static_cast<int32_t>(telem->turbo_pressure_kpa);
        case Field::TurboRpm:
            return static_cast<int32_t>(telem->turbo_rpm);
    }
    return 0;
}

void TelemGaugeScreen::refresh() {
    dirty_ = false;
    if (root_ == nullptr) {
        return;
    }

    updateFooterStatus(footer_label_, telem_, stale_, nav_, false);
    if (stale_ || telem_ == nullptr || !fieldValid(telem_)) {
        gauge_.setValue(0, true);
        return;
    }
    gauge_.setValue(fieldValue(telem_), false);
}

size_t registerPidGaugeScreens(Navigator& nav, TelemetryRegistry& telem,
                               const PanelLayout& layout, uint8_t start_page,
                               uint8_t total_pages) {
    for (size_t i = 0; i < kPidGaugeCount; ++i) {
        ScreenNavInfo page_nav;
        page_nav.index = static_cast<uint8_t>(start_page + i);
        page_nav.total = total_pages;
        g_pid_gauges[i].init(layout, page_nav, makeScreenInit(kPidGaugeDefs[i]));
        nav.addScreen(&g_pid_gauges[i]);
        telem.add(&g_pid_gauges[i]);
    }
    return kPidGaugeCount;
}

}  // namespace ui
