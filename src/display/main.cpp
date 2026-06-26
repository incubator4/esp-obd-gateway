#include <Arduino.h>

#include "app/navigator.h"
#include "app/gateway_link.h"
#include "app/telemetry.h"
#include "app/widgets/pair_success_toast.h"
#include "display.h"
#include "config_link.h"
#include "protocol.h"
#include "transport.h"

#if defined(DISPLAY_CHIP_C6)
#include "board/c6/register.h"
#elif defined(DISPLAY_CHIP_S3)
#include "board/s3/register.h"
#else
#error "Define DISPLAY_PROFILE_C6_13, DISPLAY_PROFILE_C6_147, or DISPLAY_PROFILE_S3_169"
#endif

static disp::Panel* g_panel = nullptr;
static disp::Input* g_input = nullptr;
static disp::FeatureRegistry g_features;
static ui::Navigator g_nav;

static obd::EspNow g_espnow;
static ObdTelemetry g_telemetry{};
static uint32_t g_last_rx_ms = 0;
static bool g_have_telemetry = false;
static uint32_t g_last_lvgl_tick_ms = 0;
static uint32_t g_last_disp_log_ms = 0;

static const char* linkStateLabel() {
    switch (ui::gatewayLink().state()) {
        case ui::GatewayLinkState::Pairing:
            return "pairing";
        case ui::GatewayLinkState::Paired:
            return "paired";
        case ui::GatewayLinkState::Unpaired:
        default:
            return "unpaired";
    }
}

static void logDisplayHeartbeat(uint32_t now_ms) {
    if (g_last_disp_log_ms != 0 && (now_ms - g_last_disp_log_ms) < DISP_LOG_HEARTBEAT_MS) {
        return;
    }
    g_last_disp_log_ms = now_ms;

    const obd::EspNowStats& es = g_espnow.stats();
    Serial.printf("[DISP] link=%s espnow rx=%lu ok=%lu bad=%lu short=%lu have_telem=%d\n",
                  linkStateLabel(), static_cast<unsigned long>(es.rx_raw),
                  static_cast<unsigned long>(es.rx_valid),
                  static_cast<unsigned long>(es.rx_bad_pkt),
                  static_cast<unsigned long>(es.rx_short), g_have_telemetry ? 1 : 0);
}

static void onEspNowPacket(const uint8_t mac[6], const ObdPacket& pkt) {
    if (pkt.type != MSG_TELEMETRY) {
        return;
    }
    if (!ui::gatewayLink().acceptFrom(mac)) {
        return;
    }
    ObdTelemetry telem{};
    if (!obd::packetGetPayload(&pkt, &telem, sizeof(telem))) {
        return;
    }

    const uint32_t now = millis();
    ui::gatewayLink().onTelemetry(mac, now);

    if (!ui::gatewayLink().hasGateway()) {
        return;
    }

    g_telemetry = telem;
    g_last_rx_ms = now;
    g_have_telemetry = true;
}

void setup() {
    Serial.begin(115200);
    const uint32_t serial_deadline = millis() + 3000;
    while (!Serial && millis() < serial_deadline) {
        delay(10);
    }
    delay(100);
    Serial.println();
    Serial.printf("[DISP] boot profile=%s\n", DISPLAY_PROFILE_NAME);

    g_panel = disp::createPanel();
    g_input = disp::createInput();
    if (g_panel == nullptr || g_input == nullptr ||
        !g_panel->begin() || !g_input->begin()) {
        Serial.println("[DISP] display init failed");
        return;
    }

    disp::registerFeatures(g_features);
    g_features.beginAll();

    if (!disp::lvglBegin(g_panel)) {
        Serial.println("[DISP] LVGL init failed");
        return;
    }

    g_nav.bindInput(g_input);

#if defined(DISPLAY_CHIP_C6)
    ui::registerC6Screens(g_nav, g_panel->size());
#elif defined(DISPLAY_CHIP_S3)
    ui::registerS3Screens(g_nav, g_panel->size());
#endif
    g_nav.restoreSavedScreen();

    ui::gatewayLink().begin();

    obd::EspNowConfig esp_cfg{};
    esp_cfg.channel = DISP_ESPNOW_CHANNEL;
    if (!g_espnow.begin(esp_cfg)) {
        Serial.println("[DISP] ESP-NOW init failed");
    } else {
        g_espnow.onReceive(onEspNowPacket);
        Serial.printf("[DISP] ESP-NOW ready channel=%d wifi_ch=%u\n", DISP_ESPNOW_CHANNEL,
                      g_espnow.currentChannel());
    }

    Serial.printf("[DISP] panel %ux%u ready\n", g_panel->size().width, g_panel->size().height);
    g_last_lvgl_tick_ms = millis();
}

void loop() {
    const uint32_t now = millis();
    const uint32_t lvgl_elapsed = now - g_last_lvgl_tick_ms;
    g_last_lvgl_tick_ms = now;
    disp::lvglTick(lvgl_elapsed);
    disp::lvglHandler();

    g_features.updateAll();
    g_nav.handleInput();
    g_nav.tick(now);

    ui::gatewayLink().tick(now);

    if (ui::gatewayLink().consumePairSuccess()) {
        uint8_t gw_mac[6]{};
        ui::gatewayLink().gatewayMac(gw_mac);
        ui::pairSuccessToast().show(gw_mac);
    }

    ui::pairSuccessToast().tick(now);

    if (ui::gatewayLink().isPairing()) {
        g_have_telemetry = false;
    }

    const bool linked = ui::gatewayLink().hasGateway();
    const bool stale =
        !linked || !g_have_telemetry || (now - g_last_rx_ms > LINK_TELEMETRY_STALE_MS);
    ui::telemetryRegistry().setAll(linked && g_have_telemetry ? &g_telemetry : nullptr,
                                   stale);

    logDisplayHeartbeat(now);

    delay(5);
}
