#pragma once

#include <cstddef>
#include <cstdint>

#include "diag/diagnostic.h"

namespace obd {

struct ObdPidRaw {
    uint8_t pid = 0;
    uint8_t bytes[6]{};
    size_t len = 0;
};

class EspObdII {
public:
    explicit EspObdII(ObdDiagnostic& diag);

    void setTimeout(uint32_t ms) { diag_.setTimeout(ms); }

    ObdError readPid(uint8_t pid, ObdPidRaw& out);

    uint32_t lastSuccessMs() const { return last_success_ms_; }
    uint32_t successCount() const { return success_count_; }
    uint32_t failCount() const { return fail_count_; }

private:
    ObdDiagnostic& diag_;
    uint32_t last_success_ms_ = 0;
    uint32_t success_count_ = 0;
    uint32_t fail_count_ = 0;
};

}  // namespace obd
