#include "app/screens/pid_gauges.h"

#include "app/navigator.h"
#include "app/telemetry.h"

namespace ui {

namespace {

constexpr size_t kPidGaugeCount = 10;

TelemGaugeScreen g_pid_gauges[kPidGaugeCount];

constexpr TelemGaugeScreen::Init kPidGaugeDefs[kPidGaugeCount] = {
    {
        ScreenId::EngineLoad,
        "Engine Load",
        ValueGauge::Config{"LOAD", "%", 0, 100, 0xFF6B35},
        TelemGaugeScreen::Field::EngineLoad,
        TELEM_VALID_ENGINE_LOAD,
    },
    {
        ScreenId::CoolantTemp,
        "Coolant Temp",
        ValueGauge::Config{"COOLANT", "C", -40, 120, 0x00A8FF},
        TelemGaugeScreen::Field::CoolantTemp,
        TELEM_VALID_COOLANT,
    },
    {
        ScreenId::FuelPressure,
        "Fuel Pressure",
        ValueGauge::Config{"FUEL", "kPa", 0, 765, 0xFFD166},
        TelemGaugeScreen::Field::FuelPressure,
        TELEM_VALID_FUEL_PRESSURE,
    },
    {
        ScreenId::IntakeMap,
        "Intake Pressure",
        ValueGauge::Config{"MAP", "kPa", 0, 255, 0x9B5DE5},
        TelemGaugeScreen::Field::IntakeMap,
        TELEM_VALID_INTAKE_MAP,
    },
    {
        ScreenId::TimingAdvance,
        "Ignition Advance",
        ValueGauge::Config{"TIMING", "deg", -64, 64, 0xF15BB5, 0x555555, false, 2, true},
        TelemGaugeScreen::Field::TimingAdvance,
        TELEM_VALID_TIMING_ADVANCE,
    },
    {
        ScreenId::IntakeTemp,
        "Intake Temp",
        ValueGauge::Config{"IAT", "C", -40, 80, 0x00D4AA},
        TelemGaugeScreen::Field::IntakeTemp,
        TELEM_VALID_INTAKE_TEMP,
    },
    {
        ScreenId::Maf,
        "Air Flow",
        ValueGauge::Config{"MAF", "g/s", 0, 200, 0x06D6A0, 0x555555, false, 10, true},
        TelemGaugeScreen::Field::Maf,
        TELEM_VALID_MAF,
    },
    {
        ScreenId::Throttle,
        "Throttle",
        ValueGauge::Config{"TPS", "%", 0, 100, 0xEF476F},
        TelemGaugeScreen::Field::Throttle,
        TELEM_VALID_THROTTLE,
    },
    {
        ScreenId::TurboPressure,
        "Turbo Pressure",
        ValueGauge::Config{"BOOST", "kPa", 0, 300, 0x118AB2},
        TelemGaugeScreen::Field::TurboPressure,
        TELEM_VALID_TURBO_PRESSURE,
    },
    {
        ScreenId::TurboRpm,
        "Turbo RPM",
        ValueGauge::Config{"T-RPM", "rpm", 0, 200000, 0x073B4C},
        TelemGaugeScreen::Field::TurboRpm,
        TELEM_VALID_TURBO_RPM,
    },
};

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
        const ScreenNavInfo page_nav{static_cast<uint8_t>(start_page + i), total_pages};
        g_pid_gauges[i].init(layout, page_nav, kPidGaugeDefs[i]);
        nav.addScreen(&g_pid_gauges[i]);
        telem.add(&g_pid_gauges[i]);
    }
    return kPidGaugeCount;
}

}  // namespace ui
