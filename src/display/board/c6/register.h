#pragma once

#include "app/navigator.h"
#include "app/screens/common.h"

namespace ui {

void registerC6Screens(Navigator& nav);
ObdDashboardScreen* c6ObdDashboardScreen();

}  // namespace ui
