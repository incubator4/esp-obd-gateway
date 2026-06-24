#pragma once

#include <cstddef>
#include <cstdint>

namespace ui {

/** Load paired gateway MAC from NVS. Returns false if unset. */
bool gatewayPeerLoad(uint8_t mac[6]);

/** Persist paired gateway MAC. */
void gatewayPeerSave(const uint8_t mac[6]);

/** Erase paired gateway MAC. */
void gatewayPeerClear();

/** Format MAC as AA:BB:CC:DD:EE:FF. Writes "Not paired" when mac is unset. */
void gatewayMacFormat(char* buf, size_t len, const uint8_t mac[6]);

}  // namespace ui
