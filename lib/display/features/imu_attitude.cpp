#include "features/imu_attitude.h"

#include <Arduino.h>
#include <cstdio>

#if defined(DISPLAY_PROFILE_S3_169)

#include "display_profiles.h"

#include <Wire.h>

#include <cmath>

namespace disp {

namespace {

constexpr uint8_t kRegWhoAmI = 0x00;
constexpr uint8_t kRegCtrl2 = 0x03;
constexpr uint8_t kRegCtrl7 = 0x08;
constexpr uint8_t kRegReset = 0x60;
constexpr uint8_t kRegStatus0 = 0x2E;
constexpr uint8_t kRegAxL = 0x35;

constexpr uint8_t kWhoAmIExpected = 0x05;
constexpr uint8_t kResetValue = 0xB0;

constexpr uint8_t kCandidateAddrs[] = {DISP_S3_IMU_ADDR, 0x6A};

bool i2cReadReg(uint8_t addr, uint8_t reg, uint8_t* out) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    if (Wire.requestFrom(static_cast<int>(addr), 1) != 1) {
        return false;
    }
    *out = static_cast<uint8_t>(Wire.read());
    return true;
}

bool i2cWriteReg(uint8_t addr, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

void logI2cScan() {
    Serial.println("[IMU] I2C scan (SDA=11 SCL=10):");
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; ++addr) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("[IMU]   0x%02X\n", addr);
            ++found;
        }
    }
    if (found == 0) {
        Serial.println("[IMU]   (no devices)");
    }
}

bool probeWhoAmI(uint8_t addr, uint8_t& who) {
    return i2cReadReg(addr, kRegWhoAmI, &who);
}

bool softReset(uint8_t addr) {
    if (!i2cWriteReg(addr, kRegReset, kResetValue)) {
        return false;
    }
    delay(50);
    return true;
}

bool configureAccelOnly(uint8_t addr) {
    constexpr uint8_t kCtrl2 = static_cast<uint8_t>((0x2U << 4) | 0x5U);
    if (!i2cWriteReg(addr, kRegCtrl2, kCtrl2)) {
        return false;
    }
    if (!i2cWriteReg(addr, kRegCtrl7, 0x01)) {
        return false;
    }
    delay(20);
    return true;
}

bool readAccelRaw(uint8_t addr, int16_t& ax, int16_t& ay, int16_t& az) {
    uint8_t status = 0;
    if (!i2cReadReg(addr, kRegStatus0, &status)) {
        return false;
    }
    (void)status;

    Wire.beginTransmission(addr);
    Wire.write(kRegAxL);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    if (Wire.requestFrom(static_cast<int>(addr), 6) != 6) {
        return false;
    }

    const uint8_t ax_l = static_cast<uint8_t>(Wire.read());
    const uint8_t ax_h = static_cast<uint8_t>(Wire.read());
    const uint8_t ay_l = static_cast<uint8_t>(Wire.read());
    const uint8_t ay_h = static_cast<uint8_t>(Wire.read());
    const uint8_t az_l = static_cast<uint8_t>(Wire.read());
    const uint8_t az_h = static_cast<uint8_t>(Wire.read());

    ax = static_cast<int16_t>((ax_h << 8) | ax_l);
    ay = static_cast<int16_t>((ay_h << 8) | ay_l);
    az = static_cast<int16_t>((az_h << 8) | az_l);
    return true;
}

void accelToAttitude(int16_t ax, int16_t ay, int16_t az, ImuAttitude& out) {
    constexpr float kLsbPerG = 4096.0f;
    const float fx = static_cast<float>(ax) / kLsbPerG;
    const float fy = static_cast<float>(ay) / kLsbPerG;
    const float fz = static_cast<float>(az) / kLsbPerG;

    out.roll_deg = std::atan2f(fy, fz) * (180.0f / static_cast<float>(M_PI));
    out.pitch_deg =
        std::atan2f(-fx, std::sqrtf(fy * fy + fz * fz)) * (180.0f / static_cast<float>(M_PI));
    out.yaw_deg = 0.0f;
    out.valid = true;
}

}  // namespace

bool ImuAttitudeFeature::begin() {
    ready_ = false;
    attitude_ = {};
    i2c_addr_ = 0;
    std::snprintf(status_, sizeof(status_), "IMU: not ready");

    Wire.begin(DISP_S3_I2C_SDA, DISP_S3_I2C_SCL);
    Wire.setClock(400000);

    uint8_t addr = 0;
    uint8_t who = 0;
    bool found = false;
    for (const uint8_t candidate : kCandidateAddrs) {
        if (probeWhoAmI(candidate, who)) {
            Serial.printf("[IMU] probe 0x%02X WHO_AM_I=0x%02X\n", candidate, who);
            if (who == kWhoAmIExpected) {
                addr = candidate;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        Serial.println("[IMU] QMI8658 not found (expect WHO_AM_I=0x05 at 0x6A/0x6B)");
        logI2cScan();
        std::snprintf(status_, sizeof(status_), "IMU: no QMI8658");
        return false;
    }

    if (!softReset(addr)) {
        Serial.println("[IMU] soft reset failed");
        std::snprintf(status_, sizeof(status_), "IMU: reset fail");
        return false;
    }

    uint8_t who_after = 0;
    if (!probeWhoAmI(addr, who_after) || who_after != kWhoAmIExpected) {
        Serial.printf("[IMU] WHO_AM_I after reset=0x%02X\n", who_after);
        std::snprintf(status_, sizeof(status_), "IMU: ID mismatch");
        return false;
    }

    if (!configureAccelOnly(addr)) {
        Serial.println("[IMU] accel config failed");
        std::snprintf(status_, sizeof(status_), "IMU: cfg fail");
        return false;
    }

    i2c_addr_ = addr;
    ready_ = true;
    std::snprintf(status_, sizeof(status_), "IMU live");
    Serial.printf("[IMU] QMI8658 ready addr=0x%02X\n", addr);
    return true;
}

void ImuAttitudeFeature::update() {
    if (!ready_) {
        return;
    }

    int16_t ax = 0;
    int16_t ay = 0;
    int16_t az = 0;
    if (!readAccelRaw(i2c_addr_, ax, ay, az)) {
        attitude_.valid = false;
        std::snprintf(status_, sizeof(status_), "IMU: read err");
        return;
    }

    accelToAttitude(ax, ay, az, attitude_);
    std::snprintf(status_, sizeof(status_), "IMU live");

#if defined(DISPLAY_DEBUG_BOOT)
    static uint32_t last_log_ms = 0;
    const uint32_t now = millis();
    if (now - last_log_ms >= 1000) {
        last_log_ms = now;
        Serial.printf("[IMU] raw ax=%d ay=%d az=%d roll=%.1f pitch=%.1f\n", ax, ay, az,
                      attitude_.roll_deg, attitude_.pitch_deg);
    }
#endif
}

}  // namespace disp

#else

namespace disp {

bool ImuAttitudeFeature::begin() {
    ready_ = false;
    attitude_ = {};
    std::snprintf(status_, sizeof(status_), "IMU: N/A");
    return true;
}

void ImuAttitudeFeature::update() {}

}  // namespace disp

#endif
