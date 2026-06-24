#include "diag/diagnostic.h"

#include <cstring>

namespace obd {

ObdDiagnostic::ObdDiagnostic(ObdIsoTp& isotp) : isotp_(isotp) {}

ObdError ObdDiagnostic::requestRaw(uint8_t mode, const uint8_t* payload,
                                    size_t payload_len, ObdPidValue& out) {
    uint8_t req[8]{};
    size_t req_len = 0;

    req[req_len++] = mode;
    if (payload_len > 0) {
        if (payload_len > sizeof(req) - 1) {
            return ObdError::BufferTooSmall;
        }
        std::memcpy(&req[1], payload, payload_len);
        req_len += payload_len;
    }

    uint8_t rsp[128]{};
    size_t rsp_len = 0;

    if (!isotp_.transact(req_id_, req, req_len, rsp, sizeof(rsp), &rsp_len,
                         timeout_ms_, rsp_id_)) {
        return ObdError::Timeout;
    }
    if (rsp_len < 1) {
        return ObdError::InvalidFormat;
    }

    if (rsp[0] == 0x7F) {
        out.mode = mode;
        out.pid = (payload_len > 0) ? payload[0] : 0;
        out.negative_code = (rsp_len >= 3) ? rsp[2] : 0;
        out.data_len = 0;
        return ObdError::NegativeResponse;
    }

    const uint8_t rsp_mode = rsp[0];
    if (rsp_mode != static_cast<uint8_t>(mode + 0x40)) {
        return ObdError::InvalidFormat;
    }

    out.mode = mode;
    out.data_len = (rsp_len > 1) ? rsp_len - 1 : 0;
    if (out.data_len > sizeof(out.data)) {
        return ObdError::BufferTooSmall;
    }
    if (out.data_len > 0) {
        std::memcpy(out.data, &rsp[1], out.data_len);
    }
    out.pid = (out.data_len > 0) ? out.data[0] : 0;
    out.negative_code = 0;
    return ObdError::Ok;
}

ObdError ObdDiagnostic::requestMode(ObdMode mode, uint8_t pid, ObdPidValue& out) {
    return requestRaw(static_cast<uint8_t>(mode), &pid, 1, out);
}

}  // namespace obd
