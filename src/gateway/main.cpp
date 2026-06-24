#include <Arduino.h>

#include "config_gateway.h"
#include "obd.h"

static obd::ObdCan g_can;
static obd::ObdIsoTp g_isotp(g_can);
static obd::ObdDiagnostic g_diag(g_isotp);
static obd::EspObdII g_obd(g_diag);
static obd::ObdCollectorConfig g_coll_cfg{
    GW_OBD_POLL_MS,
    GW_OBD_STALE_MS,
};
static obd::ObdCollector g_collector(g_obd, g_coll_cfg);

void setup() {
    Serial.begin(115200);
    delay(500);

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

    if (!g_can.begin(cfg)) {
        Serial.println("OBD CAN init failed");
        return;
    }

    Serial.println("OBD collector ready");
}

void loop() {
    g_collector.poll();
    g_can.recover();

    const ObdTelemetry& t = g_collector.telemetry();
    Serial.printf("RPM=%u Speed=%u Coolant=%dC Throttle=%u%% Load=%u%% "
                  "Fuel=%u%% MAF=%u.%u g/s Intake=%dC flags=0x%02X\n",
                  t.rpm, t.speed_kmh, t.coolant_c, t.throttle_pct,
                  t.engine_load_pct, t.fuel_level_pct,
                  t.maf_gps_x10 / 10U, t.maf_gps_x10 % 10U,
                  t.intake_temp_c, t.flags);

    delay(200);
}
