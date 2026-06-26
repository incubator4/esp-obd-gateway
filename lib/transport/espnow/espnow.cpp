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

void setLegacyProtocol() {
    esp_wifi_set_protocol(WIFI_IF_STA,
                          WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
}

void configureWifiRadio(uint8_t channel) {
    WiFi.persistent(false);
    WiFi.enableSTA(true);
    WiFi.mode(WIFI_STA);

    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_ps(WIFI_PS_NONE);

#if CONFIG_IDF_TARGET_ESP32C6
    // ESP32-C6 defaults may prefer Wi-Fi 6; lock to 2.4 GHz legacy for ESP-NOW.
    esp_wifi_set_band_mode(WIFI_BAND_MODE_2G_ONLY);
#endif

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    setLegacyProtocol();

    // Boost TX for bench testing (unit: 0.25 dBm, 80 ≈ 20 dBm).
    esp_wifi_set_max_tx_power(80);
}

void configureLegacyPeerRate(const uint8_t mac[6]) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
    esp_now_rate_config_t rate_cfg = {};
    rate_cfg.phymode = WIFI_PHY_MODE_11G;
    rate_cfg.rate = WIFI_PHY_RATE_6M;
    rate_cfg.ersu = false;
    rate_cfg.dcm = false;
    esp_now_set_peer_rate_config(mac, &rate_cfg);
#else
    (void)mac;
#endif
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

    configureWifiRadio(cfg_.channel);

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    esp_now_register_recv_cb(recvTrampoline);
    esp_now_register_send_cb(sendTrampoline);

    // Register broadcast peer on both ends — helps some ESP32-C6 recv paths.
    if (!addPeer(kBroadcastMac, cfg_.channel)) {
        esp_now_deinit();
        return false;
    }
    configureLegacyPeerRate(kBroadcastMac);

    s_instance = this;
    running_ = true;
    last_send_ok_ = true;
    stats_ = {};
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
    peer.channel = (channel != 0) ? channel : cfg_.channel;
    peer.encrypt = false;
    peer.ifidx = WIFI_IF_STA;
    return esp_now_add_peer(&peer) == ESP_OK;
}

bool EspNow::removePeer(const uint8_t mac[6]) {
    if (!esp_now_is_peer_exist(mac)) {
        return true;
    }
    return esp_now_del_peer(mac) == ESP_OK;
}

bool EspNow::ensureBroadcastPeer() {
    return addPeer(kBroadcastMac, cfg_.channel);
}

bool EspNow::send(const uint8_t mac[6], ObdPacket& pkt) {
    if (!running_) {
        return false;
    }
    if (!addPeer(mac, cfg_.channel)) {
        return false;
    }

    packetSeal(&pkt);
    ++stats_.tx_queue;
    return esp_now_send(mac, reinterpret_cast<uint8_t*>(&pkt), sizeof(pkt)) == ESP_OK;
}

bool EspNow::broadcast(ObdPacket& pkt) {
    return send(kBroadcastMac, pkt);
}

void EspNow::onRecv(const uint8_t mac[6], const uint8_t* data, int len) {
    ++stats_.rx_raw;
    if (!recv_cb_ || len < static_cast<int>(sizeof(ObdPacket))) {
        ++stats_.rx_short;
        return;
    }

    ObdPacket pkt;
    std::memcpy(&pkt, data, sizeof(pkt));
    if (!packetValid(&pkt)) {
        ++stats_.rx_bad_pkt;
        return;
    }

    ++stats_.rx_valid;
    recv_cb_(mac, pkt);
}

void EspNow::onSendDone(bool ok) {
    last_send_ok_ = ok;
    if (ok) {
        ++stats_.tx_ok;
    } else {
        ++stats_.tx_fail;
    }
}

uint8_t EspNow::currentChannel() const {
    uint8_t channel = 0;
    wifi_second_chan_t second = WIFI_SECOND_CHAN_NONE;
    if (esp_wifi_get_channel(&channel, &second) != ESP_OK) {
        return 0;
    }
    return channel;
}

const EspNowStats& EspNow::stats() const { return stats_; }

}  // namespace obd
