#pragma once

#include <cstdint>

#include "driver/twai.h"

namespace obd {

enum class ObdBitrate : uint32_t {
    k125K = 125000,
    k250K = 250000,
    k500K = 500000,
};

struct ObdCanPins {
    int tx = 17;
    int rx = 18;
    int rs = -1;
    bool rs_active_high = true;
};

struct ObdCanConfig {
    ObdCanPins pins;
    ObdBitrate bitrate = ObdBitrate::k500K;
};

class ObdCan {
public:
    ObdCan() = default;
    ~ObdCan();

    ObdCan(const ObdCan&) = delete;
    ObdCan& operator=(const ObdCan&) = delete;

    bool begin(const ObdCanConfig& cfg);
    void end();

    bool isRunning() const { return running_; }

    bool send(const twai_message_t& msg, uint32_t timeout_ms = 100);
    bool recv(twai_message_t& msg, uint32_t timeout_ms = 100);

    bool recover(uint32_t timeout_ms = 1000);

    twai_status_info_t status() const;

private:
    static twai_timing_config_t timingFor(ObdBitrate br);
    void configureRsPin() const;

    ObdCanConfig cfg_{};
    bool running_ = false;
};

}  // namespace obd
