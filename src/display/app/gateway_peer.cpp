#include "app/gateway_peer.h"

#include <Preferences.h>

#include <cstdio>
#include <cstring>

namespace ui {

namespace {

constexpr const char* kNamespace = "obd_display";
constexpr const char* kGatewayMacKey = "gw_mac";

bool macIsSet(const uint8_t mac[6]) {
    uint8_t zero[6]{};
    uint8_t broadcast[6]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    return std::memcmp(mac, zero, 6) != 0 && std::memcmp(mac, broadcast, 6) != 0;
}

}  // namespace

bool gatewayPeerLoad(uint8_t mac[6]) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, true)) {
        return false;
    }
    if (!prefs.isKey(kGatewayMacKey)) {
        prefs.end();
        return false;
    }

    const size_t len = prefs.getBytesLength(kGatewayMacKey);
    if (len != 6) {
        prefs.end();
        return false;
    }

    prefs.getBytes(kGatewayMacKey, mac, 6);
    prefs.end();
    return macIsSet(mac);
}

void gatewayPeerSave(const uint8_t mac[6]) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) {
        return;
    }
    prefs.putBytes(kGatewayMacKey, mac, 6);
    prefs.end();
}

void gatewayPeerClear() {
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) {
        return;
    }
    prefs.remove(kGatewayMacKey);
    prefs.end();
}

void gatewayMacFormat(char* buf, size_t len, const uint8_t mac[6]) {
    if (buf == nullptr || len == 0) {
        return;
    }
    if (!macIsSet(mac)) {
        std::snprintf(buf, len, "Not paired");
        return;
    }
    std::snprintf(buf, len, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
                  mac[4], mac[5]);
}

}  // namespace ui
