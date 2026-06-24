#include "service/collector.h"

#include <Arduino.h>

namespace obd {

constexpr uint8_t ObdCollector::kPidList[];

ObdCollector::ObdCollector(EspObdII& obd, const ObdCollectorConfig& cfg)
    : obd_(obd), cfg_(cfg) {}

bool ObdCollector::poll(uint32_t now_ms) {
    if (now_ms == 0) {
        now_ms = millis();
    }
    if (last_poll_ms_ != 0 &&
        (now_ms - last_poll_ms_) < cfg_.poll_interval_ms) {
        refreshFlags(now_ms);
        return true;
    }

    last_poll_ms_ = now_ms;
    ++poll_count_;

    uint16_t valid_mask = 0;
    for (const uint8_t pid : kPidList) {
        ObdPidRaw raw{};
        if (obd_.readPid(pid, raw) != ObdError::Ok) {
            continue;
        }
        if (applyPidToTelemetry(pid, raw.bytes, raw.len, telem_)) {
            valid_mask |= telemValidBitForPid(pid);
        }
    }
    telem_.valid_mask = valid_mask;

    refreshFlags(now_ms);
    return true;
}

void ObdCollector::refreshFlags(uint32_t now_ms) {
    const bool can_ok = obd_.successCount() > 0;
    const bool obd_connected =
        (obd_.lastSuccessMs() != 0) &&
        ((now_ms - obd_.lastSuccessMs()) <= cfg_.stale_timeout_ms);

    telem_.flags = 0;
    if (obd_connected) {
        telem_.flags |= TELEM_FLAG_OBD_CONNECTED;
    }
    if (can_ok) {
        telem_.flags |= TELEM_FLAG_CAN_OK;
    }
}

}  // namespace obd
