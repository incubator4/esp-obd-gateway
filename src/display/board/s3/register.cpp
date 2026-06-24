#include "board/s3/register.h"

#include "app/screens/common.h"
#include "board/s3/screens/attitude.h"

namespace ui {

static ObdDashboardScreen g_obd;
static SettingsScreen g_settings;
static AttitudeHorizonScreen g_attitude;

void registerS3Screens(Navigator& nav) {
    nav.addScreen(&g_obd);
    nav.addScreen(&g_attitude);
    nav.addScreen(&g_settings);
    // TODO: S3 专用：RTC 时钟、电池、蜂鸣器测试界面
}

ObdDashboardScreen* s3ObdDashboardScreen() { return &g_obd; }

}  // namespace ui
