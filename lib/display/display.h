#pragma once

/**
 * 显示硬件抽象层。
 *
 *   core/           面板、输入、板载特性
 *   panels/         Waveshare 各型号驱动（暂为占位）
 *   features/       跨板能力（如 IMU 姿态）
 *   board/factory   按 DISPLAY_BOARD_* 选择实现
 */

#include "board/factory.h"
#include "core/feature.h"
#include "core/input.h"
#include "core/lvgl_port.h"
#include "core/panel.h"
#include "features/imu_attitude.h"
