#include "link/isotp.h"

#include <Arduino.h>
#include <cstring>

namespace obd {

ObdIsoTp::ObdIsoTp(ObdCan& can) : can_(can) {}

bool ObdIsoTp::isResponseId(uint32_t id, uint32_t expect_id) const {
    if (expect_id == kFunctionalRequestId || expect_id == kResponseBase) {
        return id >= kResponseBase && id <= kResponseMax;
    }
    return id == expect_id;
}

bool ObdIsoTp::sendPayload(uint32_t can_id, uint32_t fc_expect_id,
                            const uint8_t* data, size_t len) {
    if (len == 0 || len > kMaxPayload) {
        return false;
    }

    twai_message_t frame{};
    frame.identifier = can_id;
    frame.extd = 0;
    frame.rtr = 0;
    frame.data_length_code = 8;

    if (len <= 7) {
        frame.data[0] = static_cast<uint8_t>(len & 0x0F);
        std::memcpy(&frame.data[1], data, len);
        for (size_t i = 1 + len; i < 8; ++i) {
            frame.data[i] = pad_byte_;
        }
        return can_.send(frame);
    }

    frame.data[0] = 0x10 | static_cast<uint8_t>((len >> 8) & 0x0F);
    frame.data[1] = static_cast<uint8_t>(len & 0xFF);
    const size_t first_chunk = 6;
    std::memcpy(&frame.data[2], data, first_chunk);
    for (size_t i = 2 + first_chunk; i < 8; ++i) {
        frame.data[i] = pad_byte_;
    }
    if (!can_.send(frame)) {
        return false;
    }

    const uint32_t fc_deadline = millis() + 1000;
    twai_message_t fc{};
    bool got_fc = false;
    while (millis() < fc_deadline) {
        if (!can_.recv(fc, 50)) {
            continue;
        }
        if (!isResponseId(fc.identifier, fc_expect_id)) {
            continue;
        }
        if ((fc.data[0] & 0xF0) == 0x30) {
            got_fc = true;
            break;
        }
    }
    if (!got_fc) {
        return false;
    }

    size_t offset = first_chunk;
    uint8_t seq = 1;
    while (offset < len) {
        twai_message_t cf{};
        cf.identifier = can_id;
        cf.extd = 0;
        cf.rtr = 0;
        cf.data_length_code = 8;
        cf.data[0] = static_cast<uint8_t>(0x20 | (seq & 0x0F));

        const size_t chunk = (len - offset > 7) ? 7 : (len - offset);
        std::memcpy(&cf.data[1], data + offset, chunk);
        for (size_t i = 1 + chunk; i < 8; ++i) {
            cf.data[i] = pad_byte_;
        }
        if (!can_.send(cf)) {
            return false;
        }
        offset += chunk;
        seq = (seq + 1) & 0x0F;
    }
    return true;
}

bool ObdIsoTp::sendFlowControl(uint32_t can_id) {
    twai_message_t fc{};
    fc.identifier = can_id;
    fc.extd = 0;
    fc.rtr = 0;
    fc.data_length_code = 8;
    fc.data[0] = 0x30;
    fc.data[1] = 0x00;
    fc.data[2] = 0x00;
    for (int i = 3; i < 8; ++i) {
        fc.data[i] = pad_byte_;
    }
    return can_.send(fc);
}

bool ObdIsoTp::recvPayload(uint32_t expect_id, uint32_t fc_reply_id,
                            uint8_t* out, size_t cap, size_t* out_len,
                            uint32_t timeout_ms) {
    const uint32_t deadline = millis() + timeout_ms;
    twai_message_t frame{};
    bool started = false;
    size_t total = 0;
    size_t written = 0;
    uint8_t next_seq = 1;

    while (millis() < deadline) {
        const uint32_t remain = deadline - millis();
        if (!can_.recv(frame, remain > 100 ? 100 : remain)) {
            continue;
        }
        if (!isResponseId(frame.identifier, expect_id)) {
            continue;
        }

        const uint8_t pci = frame.data[0];
        const uint8_t type = pci & 0xF0;

        if (type == 0x00) {
            const size_t len = pci & 0x0F;
            if (len == 0 || len > 7 || len > cap) {
                return false;
            }
            std::memcpy(out, &frame.data[1], len);
            *out_len = len;
            return true;
        }

        if (type == 0x10) {
            total = ((static_cast<size_t>(pci & 0x0F)) << 8) | frame.data[1];
            if (total == 0 || total > cap || total > kMaxPayload) {
                return false;
            }
            const size_t chunk = 6;
            std::memcpy(out, &frame.data[2], chunk);
            written = chunk;
            started = true;
            next_seq = 1;
            if (!sendFlowControl(fc_reply_id)) {
                return false;
            }
            if (written >= total) {
                *out_len = total;
                return true;
            }
            continue;
        }

        if (type == 0x20 && started) {
            const uint8_t seq = pci & 0x0F;
            if (seq != next_seq) {
                return false;
            }
            const size_t chunk = (total - written > 7) ? 7 : (total - written);
            std::memcpy(out + written, &frame.data[1], chunk);
            written += chunk;
            next_seq = (next_seq + 1) & 0x0F;
            if (written >= total) {
                *out_len = total;
                return true;
            }
        }
    }
    return false;
}

bool ObdIsoTp::transact(uint32_t req_id,
                         const uint8_t* req, size_t req_len,
                         uint8_t* rsp, size_t rsp_cap, size_t* rsp_len,
                         uint32_t timeout_ms, uint32_t expect_rsp) {
    if (req == nullptr || req_len == 0 || rsp == nullptr ||
        rsp_cap == 0 || rsp_len == nullptr) {
        return false;
    }

    const uint32_t rsp_id =
        (expect_rsp == kResponseBase && req_id >= kPhysicalRequestBase &&
         req_id <= kPhysicalRequestBase + 7)
            ? req_id + 8
            : expect_rsp;

    if (!sendPayload(req_id, rsp_id, req, req_len)) {
        return false;
    }
    return recvPayload(rsp_id, req_id, rsp, rsp_cap, rsp_len, timeout_ms);
}

}  // namespace obd
