#include <Arduino.h>

#include "app/navigator.h"
#include "app/telemetry.h"
#include "display.h"
#include "protocol.h"
#include "transport.h"

#if defined(DISPLAY_BOARD_C6_LCD_13)
#include "board/c6/register.h"
#include "config_display_c6.h"
#elif defined(DISPLAY_BOARD_S3_TOUCH_169)
#include "board/s3/register.h"
#include "config_display_s3.h"
#else
#error "Define DISPLAY_BOARD_C6_LCD_13 or DISPLAY_BOARD_S3_TOUCH_169"
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

static void onEspNowPacket(const uint8_t mac[6], const ObdPacket& pkt) {
    (void)mac;
    if (pkt.type != MSG_TELEMETRY) {
        return;
    }
    if (!obd::packetGetPayload(&pkt, &g_telemetry, sizeof(g_telemetry))) {
        return;
    }
    g_last_rx_ms = millis();
    g_have_telemetry = true;
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("OBD Display");

    g_panel = disp::createPanel();
    g_input = disp::createInput();
    if (g_panel == nullptr || g_input == nullptr ||
        !g_panel->begin() || !g_input->begin()) {
        Serial.println("Display init failed");
        return;
    }

    disp::registerFeatures(g_features);
    g_features.beginAll();

    if (!disp::lvglBegin(g_panel)) {
        Serial.println("LVGL init failed");
        return;
    }

    g_nav.bindInput(g_input);

#if defined(DISPLAY_BOARD_C6_LCD_13)
    ui::registerC6Screens(g_nav, g_panel->size());
#elif defined(DISPLAY_BOARD_S3_TOUCH_169)
    ui::registerS3Screens(g_nav, g_panel->size());
#endif

    obd::EspNowConfig esp_cfg{};
    esp_cfg.channel = DISP_ESPNOW_CHANNEL;
    if (!g_espnow.begin(esp_cfg)) {
        Serial.println("ESP-NOW init failed");
    } else {
        g_espnow.onReceive(onEspNowPacket);
    }

    Serial.printf("Panel %ux%u ready\n", g_panel->size().width, g_panel->size().height);
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

    const bool stale = !g_have_telemetry || (now - g_last_rx_ms > 2000);
    ui::telemetryRegistry().setAll(g_have_telemetry ? &g_telemetry : nullptr, stale);

    delay(5);
}
