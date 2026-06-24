#include <Arduino.h>

#include "config_gateway.h"
#include "config_link.h"
#include "gateway_log.h"
#include "obd.h"
#include "transport.h"

static obd::ObdCan g_can;
static obd::ObdIsoTp g_isotp(g_can);
static obd::ObdDiagnostic g_diag(g_isotp);
static obd::EspObdII g_obd(g_diag);
static obd::ObdCollectorConfig g_coll_cfg{
    GW_OBD_POLL_MS,
    GW_OBD_STALE_MS,
};
static obd::ObdCollector g_collector(g_obd, g_coll_cfg);

static obd::EspNow g_espnow;
static uint16_t g_seq = 0;
static uint32_t g_last_telem_ms = 0;
static uint32_t g_last_log_ms = 0;
static uint32_t g_telem_tx_count = 0;
static bool g_can_ok = false;
static bool g_espnow_ok = false;

static void sendPong(const uint8_t mac[6]) {
    ObdPacket pkt{};
    obd::packetInit(&pkt, MSG_PONG, g_seq++, millis());
    g_espnow.send(mac, pkt);
}

static void sendPidResponse(const uint8_t mac[6], const ObdPidRequest& req) {
    ObdPidResponse rsp{};
    rsp.pid = req.pid;
    rsp.ok = obd::telemPidValue(g_collector.telemetry(), req.pid, rsp.value) ? 1 : 0;

    ObdPacket pkt{};
    obd::packetInit(&pkt, MSG_PID_RESPONSE, g_seq++, millis());
    obd::packetSetPayload(&pkt, &rsp, sizeof(rsp));
    g_espnow.send(mac, pkt);
}

static void onEspNowPacket(const uint8_t mac[6], const ObdPacket& pkt) {
    switch (pkt.type) {
        case MSG_PING:
            gwLogf("[GW] ping from %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2],
                   mac[3], mac[4], mac[5]);
            sendPong(mac);
            break;
        case MSG_PID_REQUEST: {
            ObdPidRequest req{};
            if (obd::packetGetPayload(&pkt, &req, sizeof(req))) {
                sendPidResponse(mac, req);
            }
            break;
        }
        default:
            break;
    }
}

static void broadcastTelemetry(uint32_t now_ms) {
    if (!g_espnow_ok) {
        return;
    }
    if (g_last_telem_ms != 0 &&
        (now_ms - g_last_telem_ms) < LINK_TELEMETRY_INTERVAL_MS) {
        return;
    }
    g_last_telem_ms = now_ms;

    ObdPacket pkt{};
    obd::packetInit(&pkt, MSG_TELEMETRY, g_seq++, now_ms);
    const ObdTelemetry& telem = g_collector.telemetry();
    obd::packetSetPayload(&pkt, &telem, sizeof(telem));
    if (g_espnow.broadcast(pkt)) {
        ++g_telem_tx_count;
    }
}

static void logHeartbeat(uint32_t now_ms) {
    if (g_last_log_ms != 0 && (now_ms - g_last_log_ms) < GW_LOG_HEARTBEAT_MS) {
        return;
    }
    g_last_log_ms = now_ms;

    const ObdTelemetry& t = g_collector.telemetry();
    const obd::EspNowStats& es = g_espnow.stats();
    gwLogf("[GW] can=%s espnow=%s telem_q=%lu tx_ok=%lu tx_fail=%lu last_tx=%s rpm=%u "
           "speed=%u flags=0x%02X valid=0x%02X",
           g_can_ok ? "ok" : "fail", g_espnow_ok ? "ok" : "fail",
           static_cast<unsigned long>(g_telem_tx_count),
           static_cast<unsigned long>(es.tx_ok), static_cast<unsigned long>(es.tx_fail),
           g_espnow.lastSendOk() ? "ok" : "fail", t.rpm, t.speed_kmh, t.flags, t.valid_mask);
}

void setup() {
    gwLogBegin();
    gwLog("[GW] boot");

    obd::ObdCanConfig cfg{};
    cfg.pins.tx = GW_CAN_TX_PIN;
    cfg.pins.rx = GW_CAN_RX_PIN;
    cfg.pins.rs = GW_CAN_RS_PIN;
    if (GW_CAN_BITRATE == 250000) {
        cfg.bitrate = obd::ObdBitrate::k250K;
    } else if (GW_CAN_BITRATE == 125000) {
        cfg.bitrate = obd::ObdBitrate::k125K;
    } else {
        cfg.bitrate = obd::ObdBitrate::k500K;
    }

    g_can_ok = g_can.begin(cfg);
    if (!g_can_ok) {
        gwLog("[GW] WARN OBD CAN init failed");
    } else {
        gwLogf("[GW] CAN ready tx=%d rx=%d bitrate=%d", GW_CAN_TX_PIN, GW_CAN_RX_PIN,
               GW_CAN_BITRATE);
    }

    obd::EspNowConfig esp_cfg{};
    esp_cfg.channel = GW_ESPNOW_CHANNEL;
    g_espnow_ok = g_espnow.begin(esp_cfg);
    if (!g_espnow_ok) {
        gwLog("[GW] WARN ESP-NOW init failed");
    } else {
        g_espnow.onReceive(onEspNowPacket);
        gwLogf("[GW] ESP-NOW ready channel=%d wifi_ch=%u", GW_ESPNOW_CHANNEL,
               g_espnow.currentChannel());
    }

    if (g_can_ok && g_espnow_ok) {
        gwLog("[GW] OBD gateway ready (broadcast telemetry)");
    } else {
        gwLog("[GW] running in degraded mode — check CAN / ESP-NOW above");
    }
}

void loop() {
    const uint32_t now = millis();

    if (g_can_ok) {
        g_collector.poll(now);
        g_can.recover();
    }
    broadcastTelemetry(now);
    logHeartbeat(now);

    delay(5);
}
