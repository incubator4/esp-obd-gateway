#pragma once

#include <cstdint>

// ESP-NOW max payload is 250 bytes; keep packets compact and fixed-layout.

#define OBD_MAGIC_0 0x4F  // 'O'
#define OBD_MAGIC_1 0x42  // 'B'
#define OBD_PROTOCOL_VERSION 1

#define OBD_MAX_PAYLOAD 32

enum ObdMsgType : uint8_t {
    MSG_TELEMETRY = 0x01,    // gateway -> broadcast
    MSG_PID_REQUEST = 0x02,  // display -> gateway
    MSG_PID_RESPONSE = 0x03, // gateway -> display (unicast)
    MSG_PING = 0x04,
    MSG_PONG = 0x05,
    MSG_STATUS = 0x06,
};

enum ObdPid : uint8_t {
    PID_ENGINE_RPM = 0x0C,
    PID_VEHICLE_SPEED = 0x0D,
    PID_COOLANT_TEMP = 0x05,
    PID_THROTTLE = 0x11,
    PID_ENGINE_LOAD = 0x04,
    PID_FUEL_LEVEL = 0x2F,
    PID_MAF = 0x10,
    PID_INTAKE_TEMP = 0x0F,
};

#pragma pack(push, 1)

struct ObdTelemetry {
    uint16_t rpm;
    uint8_t speed_kmh;
    int8_t coolant_c;
    uint8_t throttle_pct;
    uint8_t engine_load_pct;
    uint8_t fuel_level_pct;
    uint16_t maf_gps_x10;
    int8_t intake_temp_c;
    uint8_t flags;  // bit0: obd_connected, bit1: can_ok
    uint8_t reserved;
};

struct ObdPidRequest {
    uint8_t pid;
    uint8_t reserved;
};

struct ObdPidResponse {
    uint8_t pid;
    uint8_t ok;
    uint32_t value;
};

struct ObdStatus {
    uint8_t obd_connected;
    uint8_t can_ok;
    uint32_t uptime_ms;
    uint32_t tx_count;
    uint32_t rx_count;
};

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

static_assert(sizeof(ObdTelemetry) <= OBD_MAX_PAYLOAD, "telemetry too large");
static_assert(sizeof(ObdPidRequest) <= OBD_MAX_PAYLOAD, "pid request too large");
static_assert(sizeof(ObdPidResponse) <= OBD_MAX_PAYLOAD, "pid response too large");
static_assert(sizeof(ObdStatus) <= OBD_MAX_PAYLOAD, "status too large");
static_assert(sizeof(ObdPacket) <= 250, "packet exceeds ESP-NOW limit");
