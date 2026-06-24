#pragma once

#include <cstddef>
#include <cstdint>

#include "bus/can.h"

namespace obd {

class ObdIsoTp {
public:
    static constexpr uint32_t kFunctionalRequestId = 0x7DF;
    static constexpr uint32_t kPhysicalRequestBase = 0x7E0;
    static constexpr uint32_t kResponseBase = 0x7E8;
    static constexpr uint32_t kResponseMax = 0x7EF;
    static constexpr size_t kMaxPayload = 4095;

    explicit ObdIsoTp(ObdCan& can);

    void setPadByte(uint8_t pad) { pad_byte_ = pad; }
    uint8_t padByte() const { return pad_byte_; }

    bool transact(uint32_t req_id,
                  const uint8_t* req, size_t req_len,
                  uint8_t* rsp, size_t rsp_cap, size_t* rsp_len,
                  uint32_t timeout_ms = 2000,
                  uint32_t expect_rsp = kResponseBase);

private:
    bool sendPayload(uint32_t can_id, uint32_t fc_expect_id,
                     const uint8_t* data, size_t len);
    bool recvPayload(uint32_t expect_id, uint32_t fc_reply_id,
                     uint8_t* out, size_t cap, size_t* out_len,
                     uint32_t timeout_ms);
    bool sendFlowControl(uint32_t can_id);
    bool isResponseId(uint32_t id, uint32_t expect_id) const;

    ObdCan& can_;
    uint8_t pad_byte_ = 0x55;
};

}  // namespace obd
