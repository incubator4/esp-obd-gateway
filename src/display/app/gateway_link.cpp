#include "app/gateway_link.h"

#include "app/gateway_peer.h"
#include "config_link.h"

#include <Arduino.h>

#include <cstring>

namespace ui {

namespace {

bool macEqual(const uint8_t a[6], const uint8_t b[6]) {
    return std::memcmp(a, b, 6) == 0;
}

void logMac(const char* label, const uint8_t mac[6]) {
    Serial.printf("%s %02X:%02X:%02X:%02X:%02X:%02X\n", label, mac[0], mac[1], mac[2],
                  mac[3], mac[4], mac[5]);
}

}  // namespace

void GatewayLink::begin() {
    loadSaved();
}

void GatewayLink::loadSaved() {
    uint8_t saved[6]{};
    if (gatewayPeerLoad(saved)) {
        std::memcpy(mac_, saved, 6);
        state_ = GatewayLinkState::Paired;
        logMac("[LINK] restored gateway", mac_);
        return;
    }
    state_ = GatewayLinkState::Unpaired;
    std::memset(mac_, 0, sizeof(mac_));
}

void GatewayLink::startPairing(uint32_t now_ms) {
    if (now_ms == 0) {
        now_ms = millis();
    }

    gatewayPeerClear();
    std::memset(mac_, 0, sizeof(mac_));
    state_ = GatewayLinkState::Pairing;
    pairing_until_ms_ = now_ms + LINK_GATEWAY_PAIR_TIMEOUT_MS;
    Serial.println("[LINK] pairing: hold released, waiting for gateway broadcast...");
}

void GatewayLink::tick(uint32_t now_ms) {
    if (now_ms == 0) {
        now_ms = millis();
    }
    if (state_ != GatewayLinkState::Pairing) {
        return;
    }
    if (now_ms >= pairing_until_ms_) {
        Serial.println("[LINK] pairing timed out");
        state_ = GatewayLinkState::Unpaired;
        pairing_until_ms_ = 0;
    }
}

bool GatewayLink::acceptFrom(const uint8_t mac[6]) const {
    if (state_ == GatewayLinkState::Pairing) {
        return true;
    }
    if (state_ == GatewayLinkState::Paired) {
        return macEqual(mac_, mac);
    }
    return false;
}

void GatewayLink::onTelemetry(const uint8_t mac[6], uint32_t now_ms) {
    (void)now_ms;
    if (state_ == GatewayLinkState::Pairing) {
        std::memcpy(mac_, mac, 6);
        gatewayPeerSave(mac_);
        state_ = GatewayLinkState::Paired;
        pairing_until_ms_ = 0;
        pair_success_pending_ = true;
        logMac("[LINK] paired with gateway", mac_);
        return;
    }
    if (state_ == GatewayLinkState::Paired && !macEqual(mac_, mac)) {
        return;
    }
}

bool GatewayLink::consumePairSuccess() {
    if (!pair_success_pending_) {
        return false;
    }
    pair_success_pending_ = false;
    return true;
}

void GatewayLink::gatewayMac(uint8_t out[6]) const {
    std::memcpy(out, mac_, 6);
}

GatewayLink& gatewayLink() {
    static GatewayLink link;
    return link;
}

}  // namespace ui
