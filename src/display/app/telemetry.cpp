#include "app/telemetry.h"

namespace ui {

bool TelemetryRegistry::add(TelemetryConsumer* consumer) {
    if (consumer == nullptr || count_ >= kMaxConsumers) {
        return false;
    }
    for (size_t i = 0; i < count_; ++i) {
        if (consumers_[i] == consumer) {
            return false;
        }
    }
    consumers_[count_++] = consumer;
    return true;
}

void TelemetryRegistry::setAll(const ObdTelemetry* telem, bool stale) {
    for (size_t i = 0; i < count_; ++i) {
        consumers_[i]->setTelemetry(telem, stale);
    }
}

TelemetryRegistry& telemetryRegistry() {
    static TelemetryRegistry registry;
    return registry;
}

}  // namespace ui
