#include "board/c6/register.h"

#include "app/screens/common.h"

namespace ui {

static ObdDashboardScreen g_obd;
static SettingsScreen g_settings;

void registerC6Screens(Navigator& nav) {
    nav.addScreen(&g_obd);
    nav.addScreen(&g_settings);
    // TODO: C6 专用界面（如 RGB 灯效、SD 卡状态）
}

ObdDashboardScreen* c6ObdDashboardScreen() { return &g_obd; }

}  // namespace ui
