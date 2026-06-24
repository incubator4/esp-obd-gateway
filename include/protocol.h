#pragma once

#include <cstddef>
#include <cstdint>

// Shared OBD telemetry protocol for Gateway ↔ Display over ESP-NOW.
//
// Transport: ESP-NOW (max payload 250 bytes). All multi-byte integers are
// little-endian. Structs use #pragma pack(1).
//
// Typical flow:
//   Gateway polls OBD-II Mode 01 PIDs, fills ObdTelemetry, wraps it in
//   ObdPacket (type MSG_TELEMETRY), and broadcasts ~10 Hz.
//   Display validates the packet, unpacks ObdTelemetry, and drives UI gauges.

#define OBD_MAGIC_0 0x4F  // 'O'
#define OBD_MAGIC_1 0x42  // 'B'
#define OBD_PROTOCOL_VERSION 1

#define OBD_MAX_PAYLOAD 32

// ---------------------------------------------------------------------------
// Message types (ObdPacket.type)
// ---------------------------------------------------------------------------

enum ObdMsgType : uint8_t {
    MSG_TELEMETRY = 0x01,     // gateway -> broadcast: ObdTelemetry snapshot
    MSG_PID_REQUEST = 0x02,   // display -> gateway: on-demand PID read
    MSG_PID_RESPONSE = 0x03,  // gateway -> display (unicast): ObdPidResponse
    MSG_PING = 0x04,          // either -> either: link probe
    MSG_PONG = 0x05,          // reply to MSG_PING
    MSG_STATUS = 0x06,        // gateway -> display: ObdStatus diagnostics
};

// ---------------------------------------------------------------------------
// OBD-II Mode 01 PIDs collected by the gateway
// ---------------------------------------------------------------------------

enum ObdPid : uint8_t {
    PID_ENGINE_LOAD = 0x04,   // Calculated engine load, 0–100 %
    PID_COOLANT_TEMP = 0x05,  // Engine coolant temperature, °C
    PID_INTAKE_TEMP = 0x0F,   // Intake air temperature, °C
    PID_MAF = 0x10,           // Mass air flow, g/s (stored as ×10)
    PID_THROTTLE = 0x11,      // Throttle position, 0–100 %
    PID_ENGINE_RPM = 0x0C,    // Engine speed, rpm
    PID_VEHICLE_SPEED = 0x0D, // Vehicle speed, km/h
    PID_FUEL_LEVEL = 0x2F,    // Fuel tank level, 0–100 %
};

// ---------------------------------------------------------------------------
// ObdTelemetry.valid_mask — one bit per field below (set when PID read OK)
// ---------------------------------------------------------------------------

enum ObdTelemValid : uint8_t {
    TELEM_VALID_RPM = 1 << 0,
    TELEM_VALID_SPEED = 1 << 1,
    TELEM_VALID_COOLANT = 1 << 2,
    TELEM_VALID_THROTTLE = 1 << 3,
    TELEM_VALID_ENGINE_LOAD = 1 << 4,
    TELEM_VALID_FUEL_LEVEL = 1 << 5,
    TELEM_VALID_MAF = 1 << 6,
    TELEM_VALID_INTAKE_TEMP = 1 << 7,
    TELEM_VALID_ALL = 0xFF,
};

// ---------------------------------------------------------------------------
// ObdTelemetry.flags — link / bus health (not per-PID validity)
// ---------------------------------------------------------------------------

enum ObdTelemFlags : uint8_t {
    TELEM_FLAG_OBD_CONNECTED = 1 << 0,  // recent successful OBD response
    TELEM_FLAG_CAN_OK = 1 << 1,         // at least one CAN transaction succeeded
};

// Returns the TELEM_VALID_* bit for a supported ObdPid, or 0 if unknown.
inline uint8_t telemValidBitForPid(uint8_t pid) {
    switch (pid) {
        case PID_ENGINE_RPM:
            return TELEM_VALID_RPM;
        case PID_VEHICLE_SPEED:
            return TELEM_VALID_SPEED;
        case PID_COOLANT_TEMP:
            return TELEM_VALID_COOLANT;
        case PID_THROTTLE:
            return TELEM_VALID_THROTTLE;
        case PID_ENGINE_LOAD:
            return TELEM_VALID_ENGINE_LOAD;
        case PID_FUEL_LEVEL:
            return TELEM_VALID_FUEL_LEVEL;
        case PID_MAF:
            return TELEM_VALID_MAF;
        case PID_INTAKE_TEMP:
            return TELEM_VALID_INTAKE_TEMP;
        default:
            return 0;
    }
}

#pragma pack(push, 1)

// Periodic OBD snapshot (MSG_TELEMETRY payload, 12 bytes).
//
// Field decoding from raw OBD Mode 01 response bytes (after PID byte):
//
//   rpm           : ((A<<8)|B) / 4
//   speed_kmh     : A
//   coolant_c     : A - 40
//   throttle_pct  : A * 100 / 255
//   engine_load_pct: A * 100 / 255
//   fuel_level_pct: A * 100 / 255
//   maf_gps_x10   : ((A<<8)|B) * 10 / 100   (grams/sec × 10)
//   intake_temp_c : A - 40
//
// UI should treat a field as usable only when its valid_mask bit is set.
// Unset fields may retain the previous value from an earlier poll cycle.
struct ObdTelemetry {
    uint16_t rpm;             // 0–8031 (OBD max before /4)
    uint8_t speed_kmh;        // 0–255 km/h
    int8_t coolant_c;         // typically −40..215 °C
    uint8_t throttle_pct;     // 0–100
    uint8_t engine_load_pct;  // 0–100
    uint8_t fuel_level_pct;   // 0–100
    uint16_t maf_gps_x10;     // MAF in g/s × 10 (e.g. 125 = 12.5 g/s)
    int8_t intake_temp_c;     // typically −40..215 °C
    uint8_t flags;            // ObdTelemFlags
    uint8_t valid_mask;       // ObdTelemValid
};

// On-demand PID read (MSG_PID_REQUEST payload).
struct ObdPidRequest {
    uint8_t pid;       // ObdPid
    uint8_t reserved;
};

// On-demand PID reply (MSG_PID_RESPONSE payload).
// value uses the same scaled encoding as ObdTelemetry fields:
//   RPM → full uint32 rpm; speed → low 8 bits; temps → int32 °C;
//   percents → 0–100; MAF → g/s × 10.
struct ObdPidResponse {
    uint8_t pid;
    uint8_t ok;        // 1 = read + decode succeeded
    uint32_t value;
};

// Gateway diagnostics (MSG_STATUS payload).
struct ObdStatus {
    uint8_t obd_connected;
    uint8_t can_ok;
    uint32_t uptime_ms;
    uint32_t tx_count;
    uint32_t rx_count;
};

// Wire envelope for every ESP-NOW frame (44 bytes).
//
//  offset  field
//  ------  -----
//  0..1    magic ('O','B')
//  2       version (= OBD_PROTOCOL_VERSION)
//  3       type (ObdMsgType)
//  4..5    seq (monotonic per sender)
//  6..9    ts_ms (sender millis() at pack time)
//  10      len (payload bytes used in data[])
//  11..42  data[len] (padded to OBD_MAX_PAYLOAD in struct)
//  43      crc8 over bytes 0..42 (CRC-8/MAXIM poly 0x31, init 0xFF)
struct ObdPacket {
    uint8_t magic[2];
    uint8_t version;
    uint8_t type;
    uint16_t seq;
    uint32_t ts_ms;
    uint8_t len;
    uint8_t data[OBD_MAX_PAYLOAD];
    uint8_t crc;
};

#pragma pack(pop)

static_assert(sizeof(ObdTelemetry) == 12, "ObdTelemetry layout changed");
static_assert(sizeof(ObdTelemetry) <= OBD_MAX_PAYLOAD, "telemetry too large");
static_assert(sizeof(ObdPidRequest) <= OBD_MAX_PAYLOAD, "pid request too large");
static_assert(sizeof(ObdPidResponse) <= OBD_MAX_PAYLOAD, "pid response too large");
static_assert(sizeof(ObdStatus) <= OBD_MAX_PAYLOAD, "status too large");
static_assert(sizeof(ObdPacket) <= 250, "packet exceeds ESP-NOW limit");
