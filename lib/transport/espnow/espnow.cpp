#include "espnow/espnow.h"

#include <cstring>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

namespace obd {

EspNow* EspNow::s_instance = nullptr;

namespace {

constexpr uint8_t kBroadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)

void recvTrampoline(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (info) {
        EspNow::handleRecv(info->src_addr, data, len);
    }
}

#else

void recvTrampoline(const uint8_t* mac, const uint8_t* data, int len) {
    EspNow::handleRecv(mac, data, len);
}

#endif

void sendTrampoline(const uint8_t* mac, esp_now_send_status_t status) {
    (void)mac;
    EspNow::handleSend(status == ESP_NOW_SEND_SUCCESS);
}

}  // namespace

void EspNow::handleRecv(const uint8_t mac[6], const uint8_t* data, int len) {
    if (s_instance) {
        s_instance->onRecv(mac, data, len);
    }
}

void EspNow::handleSend(bool ok) {
    if (s_instance) {
        s_instance->onSendDone(ok);
    }
}

EspNow::~EspNow() { end(); }

bool EspNow::begin(const EspNowConfig& cfg) {
    end();
    cfg_ = cfg;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    esp_wifi_set_channel(cfg_.channel, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    esp_now_register_recv_cb(recvTrampoline);
    esp_now_register_send_cb(sendTrampoline);

    s_instance = this;
    running_ = true;
    last_send_ok_ = true;
    return true;
}

void EspNow::end() {
    if (!running_) {
        return;
    }

    esp_now_deinit();
    if (s_instance == this) {
        s_instance = nullptr;
    }
    running_ = false;
}

void EspNow::onReceive(EspNowRecvFn cb) { recv_cb_ = std::move(cb); }

bool EspNow::addPeer(const uint8_t mac[6], uint8_t channel) {
    if (esp_now_is_peer_exist(mac)) {
        return true;
    }

    esp_now_peer_info_t peer = {};
    std::memcpy(peer.peer_addr, mac, 6);
    peer.channel = channel;
    peer.encrypt = false;
    return esp_now_add_peer(&peer) == ESP_OK;
}

bool EspNow::removePeer(const uint8_t mac[6]) {
    if (!esp_now_is_peer_exist(mac)) {
        return true;
    }
    return esp_now_del_peer(mac) == ESP_OK;
}

bool EspNow::ensureBroadcastPeer() {
    return addPeer(kBroadcastMac, 0);
}

bool EspNow::send(const uint8_t mac[6], ObdPacket& pkt) {
    if (!running_) {
        return false;
    }
    if (!addPeer(mac, 0)) {
        return false;
    }

    packetSeal(&pkt);
    return esp_now_send(mac, reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt)) == ESP_OK;
}

bool EspNow::broadcast(ObdPacket& pkt) {
    if (!ensureBroadcastPeer()) {
        return false;
    }
    return send(kBroadcastMac, pkt);
}

void EspNow::onRecv(const uint8_t mac[6], const uint8_t* data, int len) {
    if (!recv_cb_ || len < static_cast<int>(sizeof(ObdPacket))) {
        return;
    }

    ObdPacket pkt;
    std::memcpy(&pkt, data, sizeof(pkt));
    if (!packetValid(&pkt)) {
        return;
    }
    recv_cb_(mac, pkt);
}

void EspNow::onSendDone(bool ok) { last_send_ok_ = ok; }

}  // namespace obd
