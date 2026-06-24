#pragma once

#include <cstddef>
#include <cstdint>

#include "protocol.h"
#include "service/obd_ii.h"
#include "service/units.h"

namespace obd {

struct ObdCollectorConfig {
    uint32_t poll_interval_ms = 100;
    uint32_t stale_timeout_ms = 3000;
};

class ObdCollector {
public:
    ObdCollector(EspObdII& obd, const ObdCollectorConfig& cfg = {});

    bool poll(uint32_t now_ms = 0);

    const ObdTelemetry& telemetry() const { return telem_; }
    ObdTelemetry& telemetry() { return telem_; }

    uint32_t pollCount() const { return poll_count_; }
    uint32_t lastPollMs() const { return last_poll_ms_; }

private:
    void refreshFlags(uint32_t now_ms);

    EspObdII& obd_;
    ObdCollectorConfig cfg_;
    ObdTelemetry telem_{};
    uint32_t last_poll_ms_ = 0;
    uint32_t poll_count_ = 0;

    static constexpr uint8_t kPidList[] = {
        PID_ENGINE_RPM,
        PID_VEHICLE_SPEED,
        PID_COOLANT_TEMP,
        PID_THROTTLE,
        PID_ENGINE_LOAD,
        PID_FUEL_LEVEL,
        PID_MAF,
        PID_INTAKE_TEMP,
    };
};

}  // namespace obd
