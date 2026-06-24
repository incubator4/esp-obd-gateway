#pragma once

#include <cstddef>
#include <cstdint>

#include "protocol.h"

namespace obd {

struct ObdUnitResult {
    bool ok = false;
};

bool unitFromRpm(const uint8_t* raw, size_t len, uint16_t& rpm);
bool unitFromSpeedKmh(const uint8_t* raw, size_t len, uint8_t& kmh);
bool unitFromCoolantC(const uint8_t* raw, size_t len, int8_t& celsius);
bool unitFromPercent255(const uint8_t* raw, size_t len, uint8_t& pct);
bool unitFromMafGpsX10(const uint8_t* raw, size_t len, uint16_t& gps_x10);
bool unitFromIntakeTempC(const uint8_t* raw, size_t len, int8_t& celsius);
bool unitFromFuelPressureKpa(const uint8_t* raw, size_t len, uint16_t& kpa);
bool unitFromKpa(const uint8_t* raw, size_t len, uint8_t& kpa);
bool unitFromTimingDegX2(const uint8_t* raw, size_t len, int8_t& deg_x2);
bool unitFromTurboPressureKpa(const uint8_t* raw, size_t len, uint16_t& kpa);

bool applyPidToTelemetry(uint8_t pid, const uint8_t* raw, size_t len,
                         ObdTelemetry& telem);

bool telemPidValue(const ObdTelemetry& telem, uint8_t pid, uint32_t& value);

}  // namespace obd
