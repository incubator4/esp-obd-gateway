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

private:
    bool ready_ = false;
    ImuAttitude attitude_{};
};

}  // namespace disp
