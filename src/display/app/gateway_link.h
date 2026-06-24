#pragma once

#include "protocol.h"

#include <cstdint>

namespace ui {

enum class GatewayLinkState : uint8_t {
    Unpaired,
    Pairing,
    Paired,
};

class GatewayLink {
public:
    void begin();

    /** Long-press BOOT: clear saved MAC and listen for the next gateway. */
    void startPairing(uint32_t now_ms = 0);

    void tick(uint32_t now_ms = 0);

    /** True if this sender should be accepted (pairing window or matched MAC). */
    bool acceptFrom(const uint8_t mac[6]) const;

    /** Call after a valid MSG_TELEMETRY payload is decoded. */
    void onTelemetry(const uint8_t mac[6], uint32_t now_ms = 0);

    GatewayLinkState state() const { return state_; }
    bool hasGateway() const { return state_ == GatewayLinkState::Paired; }
    bool isPairing() const { return state_ == GatewayLinkState::Pairing; }

    void gatewayMac(uint8_t out[6]) const;

    /** True once after a pairing window succeeds; cleared by consumePairSuccess(). */
    bool consumePairSuccess();

private:
    void loadSaved();

    GatewayLinkState state_ = GatewayLinkState::Unpaired;
    uint8_t mac_[6]{};
    uint32_t pairing_until_ms_ = 0;
    bool pair_success_pending_ = false;
};

GatewayLink& gatewayLink();

}  // namespace ui
