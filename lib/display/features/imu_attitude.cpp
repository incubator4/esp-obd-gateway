#include "features/imu_attitude.h"

namespace disp {

bool ImuAttitudeFeature::begin() {
    // TODO: 初始化 QMI8658（I2C 0x6B），配置加速度/陀螺仪量程
    ready_ = false;
    attitude_ = {};
    return true;
}

void ImuAttitudeFeature::update() {
    if (!ready_) {
        return;
    }
    // TODO: 读取 QMI8658，互补滤波或 Madgwick 融合 roll/pitch/yaw
    attitude_.valid = false;
}

}  // namespace disp
