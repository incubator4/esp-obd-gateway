#pragma once

#include <cstdint>
#include <functional>

#include "packet/packet.h"
#include "protocol.h"

namespace obd {

using EspNowRecvFn = std::function<void(const uint8_t mac[6], const ObdPacket& pkt)>;

struct EspNowConfig {
    uint8_t channel = 1;
};

class EspNow {
public:
    EspNow() = default;
    ~EspNow();

    EspNow(const EspNow&) = delete;
    EspNow& operator=(const EspNow&) = delete;

    bool begin(const EspNowConfig& cfg = {});
    void end();

    bool isRunning() const { return running_; }

    void onReceive(EspNowRecvFn cb);

    bool send(const uint8_t mac[6], ObdPacket& pkt);
    bool broadcast(ObdPacket& pkt);

    bool addPeer(const uint8_t mac[6], uint8_t channel = 0);
    bool removePeer(const uint8_t mac[6]);

    bool lastSendOk() const { return last_send_ok_; }

    static void handleRecv(const uint8_t mac[6], const uint8_t* data, int len);
    static void handleSend(bool ok);

private:
    void onRecv(const uint8_t mac[6], const uint8_t* data, int len);
    void onSendDone(bool ok);
    bool ensureBroadcastPeer();

    static EspNow* s_instance;

    EspNowConfig cfg_{};
    EspNowRecvFn recv_cb_;
    bool running_ = false;
    bool last_send_ok_ = true;
};

}  // namespace obd
