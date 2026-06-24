#pragma once

/**
 * OBD 协议栈总入口。
 *
 *   driver.h           — CAN / ISO-TP / 诊断
 *   service/obd_ii     — PID 读取
 *   service/collector  — 轮询 + ObdTelemetry
 *   service/units      — 单位换算
 */

#include "driver.h"
#include "service/collector.h"
#include "service/obd_ii.h"
#include "service/units.h"
