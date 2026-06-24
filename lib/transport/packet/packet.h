#pragma once

#include <cstring>

#include "crc/crc.h"
#include "protocol.h"

namespace obd {

inline void packetInit(ObdPacket* pkt, ObdMsgType type, uint16_t seq, uint32_t ts_ms) {
    pkt->magic[0] = OBD_MAGIC_0;
    pkt->magic[1] = OBD_MAGIC_1;
    pkt->version = OBD_PROTOCOL_VERSION;
    pkt->type = type;
    pkt->seq = seq;
    pkt->ts_ms = ts_ms;
    pkt->len = 0;
    std::memset(pkt->data, 0, sizeof(pkt->data));
    pkt->crc = 0;
}

inline void packetSetPayload(ObdPacket* pkt, const void* payload, uint8_t len) {
    if (len > OBD_MAX_PAYLOAD) {
        len = OBD_MAX_PAYLOAD;
    }
    pkt->len = len;
    std::memcpy(pkt->data, payload, len);
}

inline void packetSeal(ObdPacket* pkt) {
    pkt->crc = crc8(reinterpret_cast<const uint8_t*>(pkt), offsetof(ObdPacket, crc));
}

inline bool packetValid(const ObdPacket* pkt) {
    if (pkt->magic[0] != OBD_MAGIC_0 || pkt->magic[1] != OBD_MAGIC_1) {
        return false;
    }
    if (pkt->version != OBD_PROTOCOL_VERSION) {
        return false;
    }
    if (pkt->len > OBD_MAX_PAYLOAD) {
        return false;
    }
    const uint8_t expected =
        crc8(reinterpret_cast<const uint8_t*>(pkt), offsetof(ObdPacket, crc));
    return pkt->crc == expected;
}

inline bool packetGetPayload(const ObdPacket* pkt, void* out, size_t out_size) {
    if (pkt->len > out_size) {
        return false;
    }
    std::memcpy(out, pkt->data, pkt->len);
    return true;
}

}  // namespace obd
