#include "service/units.h"

namespace obd {

bool unitFromRpm(const uint8_t* raw, size_t len, uint16_t& rpm) {
    if (raw == nullptr || len < 2) {
        return false;
    }
    const uint32_t value =
        ((static_cast<uint32_t>(raw[0]) << 8) | raw[1]) / 4U;
    rpm = (value > 0xFFFFU) ? 0xFFFFU : static_cast<uint16_t>(value);
    return true;
}

bool unitFromSpeedKmh(const uint8_t* raw, size_t len, uint8_t& kmh) {
    if (raw == nullptr || len < 1) {
        return false;
    }
    kmh = raw[0];
    return true;
}

bool unitFromCoolantC(const uint8_t* raw, size_t len, int8_t& celsius) {
    if (raw == nullptr || len < 1) {
        return false;
    }
    const int16_t value = static_cast<int16_t>(raw[0]) - 40;
    if (value < -128 || value > 127) {
        return false;
    }
    celsius = static_cast<int8_t>(value);
    return true;
}

bool unitFromPercent255(const uint8_t* raw, size_t len, uint8_t& pct) {
    if (raw == nullptr || len < 1) {
        return false;
    }
    pct = static_cast<uint8_t>((static_cast<uint16_t>(raw[0]) * 100U + 127U) / 255U);
    return true;
}

bool unitFromMafGpsX10(const uint8_t* raw, size_t len, uint16_t& gps_x10) {
    if (raw == nullptr || len < 2) {
        return false;
    }
    const uint32_t grams_per_sec_x10 =
        ((static_cast<uint32_t>(raw[0]) << 8) | raw[1]) * 10U / 100U;
    gps_x10 =
        (grams_per_sec_x10 > 0xFFFFU) ? 0xFFFFU : static_cast<uint16_t>(grams_per_sec_x10);
    return true;
}

bool unitFromIntakeTempC(const uint8_t* raw, size_t len, int8_t& celsius) {
    return unitFromCoolantC(raw, len, celsius);
}

bool applyPidToTelemetry(uint8_t pid, const uint8_t* raw, size_t len,
                         ObdTelemetry& telem) {
    switch (pid) {
        case PID_ENGINE_RPM:
            return unitFromRpm(raw, len, telem.rpm);
        case PID_VEHICLE_SPEED:
            return unitFromSpeedKmh(raw, len, telem.speed_kmh);
        case PID_COOLANT_TEMP:
            return unitFromCoolantC(raw, len, telem.coolant_c);
        case PID_THROTTLE:
            return unitFromPercent255(raw, len, telem.throttle_pct);
        case PID_ENGINE_LOAD:
            return unitFromPercent255(raw, len, telem.engine_load_pct);
        case PID_FUEL_LEVEL:
            return unitFromPercent255(raw, len, telem.fuel_level_pct);
        case PID_MAF:
            return unitFromMafGpsX10(raw, len, telem.maf_gps_x10);
        case PID_INTAKE_TEMP:
            return unitFromIntakeTempC(raw, len, telem.intake_temp_c);
        default:
            return false;
    }
}

bool telemPidValue(const ObdTelemetry& telem, uint8_t pid, uint32_t& value) {
    switch (pid) {
        case PID_ENGINE_RPM:
            if ((telem.valid_mask & TELEM_VALID_RPM) == 0) {
                return false;
            }
            value = telem.rpm;
            return true;
        case PID_VEHICLE_SPEED:
            if ((telem.valid_mask & TELEM_VALID_SPEED) == 0) {
                return false;
            }
            value = telem.speed_kmh;
            return true;
        case PID_COOLANT_TEMP:
            if ((telem.valid_mask & TELEM_VALID_COOLANT) == 0) {
                return false;
            }
            value = static_cast<uint32_t>(static_cast<int32_t>(telem.coolant_c));
            return true;
        case PID_THROTTLE:
            if ((telem.valid_mask & TELEM_VALID_THROTTLE) == 0) {
                return false;
            }
            value = telem.throttle_pct;
            return true;
        case PID_ENGINE_LOAD:
            if ((telem.valid_mask & TELEM_VALID_ENGINE_LOAD) == 0) {
                return false;
            }
            value = telem.engine_load_pct;
            return true;
        case PID_FUEL_LEVEL:
            if ((telem.valid_mask & TELEM_VALID_FUEL_LEVEL) == 0) {
                return false;
            }
            value = telem.fuel_level_pct;
            return true;
        case PID_MAF:
            if ((telem.valid_mask & TELEM_VALID_MAF) == 0) {
                return false;
            }
            value = telem.maf_gps_x10;
            return true;
        case PID_INTAKE_TEMP:
            if ((telem.valid_mask & TELEM_VALID_INTAKE_TEMP) == 0) {
                return false;
            }
            value = static_cast<uint32_t>(static_cast<int32_t>(telem.intake_temp_c));
            return true;
        default:
            return false;
    }
}

}  // namespace obd
