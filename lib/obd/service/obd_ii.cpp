#include "service/obd_ii.h"

#include <Arduino.h>
#include <cstring>

namespace obd {

EspObdII::EspObdII(ObdDiagnostic& diag) : diag_(diag) {}

ObdError EspObdII::readPid(uint8_t pid, ObdPidRaw& out) {
    out.pid = pid;
    out.len = 0;

    ObdPidValue val{};
    const ObdError err =
        diag_.requestMode(ObdMode::ShowCurrentData, pid, val);

    if (err != ObdError::Ok) {
        ++fail_count_;
        return err;
    }
    if (val.data_len < 1 || val.data[0] != pid) {
        ++fail_count_;
        return ObdError::InvalidFormat;
    }

    const size_t payload_len = val.data_len - 1;
    if (payload_len > sizeof(out.bytes)) {
        ++fail_count_;
        return ObdError::BufferTooSmall;
    }

    if (payload_len > 0) {
        std::memcpy(out.bytes, &val.data[1], payload_len);
    }
    out.len = payload_len;

    last_success_ms_ = millis();
    ++success_count_;
    return ObdError::Ok;
}

}  // namespace obd
