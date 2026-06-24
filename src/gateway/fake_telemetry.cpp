#include "fake_telemetry.h"

#include <cmath>

namespace gw {

void FakeTelemetry::update(uint32_t now_ms) {
    // ~30 s driving cycle: idle -> accel -> cruise -> decel.
    constexpr uint32_t kCycleMs = 30000;
    const float phase =
        static_cast<float>(now_ms % kCycleMs) / static_cast<float>(kCycleMs) * 2.0f *
        static_cast<float>(M_PI);
    const float wave = (std::sinf(phase) + 1.0f) * 0.5f;  // 0..1

    telem_.rpm = static_cast<uint16_t>(800 + wave * 5200);
    telem_.speed_kmh = static_cast<uint8_t>(telem_.rpm / 40);
    telem_.coolant_c = static_cast<int8_t>(88 + std::sinf(phase * 0.3f) * 3.0f);
    telem_.throttle_pct = static_cast<uint8_t>(10 + wave * 70);
    telem_.engine_load_pct = static_cast<uint8_t>(15 + wave * 60);
    telem_.fuel_level_pct = 72;
    telem_.maf_gps_x10 = static_cast<uint16_t>((telem_.rpm / 100) * 25);
    telem_.intake_temp_c = static_cast<int8_t>(28 + std::sinf(phase * 0.5f) * 5.0f);
    telem_.fuel_pressure_kpa = static_cast<uint16_t>(350 + wave * 100);
    telem_.intake_map_kpa = static_cast<uint8_t>(30 + wave * 80);
    telem_.timing_advance_deg_x2 = static_cast<int8_t>(10 + wave * 30);
    telem_.turbo_pressure_kpa = static_cast<uint16_t>(wave * 180);
    telem_.turbo_rpm = static_cast<uint16_t>(wave * 120000);

    telem_.flags = TELEM_FLAG_OBD_CONNECTED;
    telem_.valid_mask = TELEM_VALID_ALL;
}

}  // namespace gw
