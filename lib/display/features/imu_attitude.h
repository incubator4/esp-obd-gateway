#pragma once

#include "core/feature.h"

namespace disp {

/** 六轴姿态（飞机地平线 / 人工地平仪用） */
struct ImuAttitude {
    float roll_deg = 0.0f;
    float pitch_deg = 0.0f;
    float yaw_deg = 0.0f;
    bool valid = false;
};

class ImuAttitudeFeature : public Feature {
public:
    FeatureId id() const override { return FeatureId::ImuAttitude; }

    bool begin() override;
    void update() override;
    bool available() const override { return ready_; }

    const ImuAttitude& attitude() const { return attitude_; }
    /** 初始化失败或未就绪时的简短说明（供 UI / 串口） */
    const char* statusText() const { return status_; }

private:
    bool ready_ = false;
    uint8_t i2c_addr_ = 0;
    ImuAttitude attitude_{};
    char status_[48] = "IMU: not ready";
};

}  // namespace disp
