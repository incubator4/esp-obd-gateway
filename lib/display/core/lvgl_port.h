#pragma once

#include "core/panel.h"

namespace disp {

/** 将 LVGL 9 显示驱动绑定到 Panel::flushArea() */
bool lvglBegin(Panel* panel);
void lvglTick(uint32_t elapsed_ms);
void lvglHandler();

}  // namespace disp
