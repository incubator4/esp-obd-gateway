#pragma once

#include "app/navigator.h"
#include "app/screens/common.h"

namespace ui {

void registerS3Screens(Navigator& nav);
ObdDashboardScreen* s3ObdDashboardScreen();

}  // namespace ui
