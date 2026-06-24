#pragma once

#include <cstdint>

#include "protocol.h"

namespace gw {

// Generates animated ObdTelemetry for gateway/display debugging without OBD.
class FakeTelemetry {
public:
    void update(uint32_t now_ms);

    const ObdTelemetry& telemetry() const { return telem_; }

private:
    ObdTelemetry telem_{};
};

}  // namespace gw
