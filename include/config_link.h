#pragma once

/** Gateway telemetry broadcast interval (ms). */
#define LINK_TELEMETRY_INTERVAL_MS 100

/** Display: hold BOOT this long to enter gateway pairing mode (ms). */
#define LINK_GATEWAY_PAIR_HOLD_MS 2000

/** Display: pairing window — accept first telemetry in this period (ms). */
#define LINK_GATEWAY_PAIR_TIMEOUT_MS 30000

/** Display: no telemetry for this long → UI shows stale (ms). */
#define LINK_TELEMETRY_STALE_MS 2000

/** Display: pairing success toast visible duration (ms). */
#define LINK_PAIR_SUCCESS_TOAST_MS 1600
