#pragma once

#include "protocol.h"

#include <cstddef>

namespace ui {

/** 接收 ObdTelemetry 更新的界面 */
class TelemetryConsumer {
public:
    virtual ~TelemetryConsumer() = default;
    virtual void setTelemetry(const ObdTelemetry* telem, bool stale) = 0;
};

class TelemetryRegistry {
public:
    bool add(TelemetryConsumer* consumer);
    void setAll(const ObdTelemetry* telem, bool stale);

private:
    static constexpr size_t kMaxConsumers = 8;
    TelemetryConsumer* consumers_[kMaxConsumers]{};
    size_t count_ = 0;
};

TelemetryRegistry& telemetryRegistry();

}  // namespace ui
