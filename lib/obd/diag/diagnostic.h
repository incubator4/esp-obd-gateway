#pragma once

#include <cstddef>
#include <cstdint>

#include "link/isotp.h"

namespace obd {

enum class ObdMode : uint8_t {
    ShowCurrentData = 0x01,
    ShowFreezeFrame = 0x02,
    ShowStoredDtc = 0x03,
    ClearDtc = 0x04,
    TestResultsO2 = 0x05,
    TestResultsOther = 0x06,
    ShowPendingDtc = 0x07,
    ControlOperation = 0x08,
    VehicleInfo = 0x09,
};

enum class ObdError : int8_t {
    Ok = 0,
    Timeout = -1,
    BusError = -2,
    NegativeResponse = -3,
    InvalidFormat = -4,
    BufferTooSmall = -5,
};

struct ObdPidValue {
    uint8_t mode = 0;
    uint8_t pid = 0;
    uint8_t data[64]{};
    size_t data_len = 0;
    uint8_t negative_code = 0;
};

class ObdDiagnostic {
public:
    explicit ObdDiagnostic(ObdIsoTp& isotp);

    void setRequestId(uint32_t id) { req_id_ = id; }
    void setResponseId(uint32_t id) { rsp_id_ = id; }
    void setTimeout(uint32_t ms) { timeout_ms_ = ms; }

    ObdError requestMode(ObdMode mode, uint8_t pid, ObdPidValue& out);
    ObdError requestRaw(uint8_t mode, const uint8_t* payload, size_t payload_len,
                        ObdPidValue& out);

private:
    ObdIsoTp& isotp_;
    uint32_t req_id_ = ObdIsoTp::kFunctionalRequestId;
    uint32_t rsp_id_ = ObdIsoTp::kResponseBase;
    uint32_t timeout_ms_ = 2000;
};

}  // namespace obd
