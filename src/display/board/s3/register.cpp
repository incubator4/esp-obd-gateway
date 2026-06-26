#include "board/s3/register.h"

#include "app/layout.h"
#include "app/navigator.h"
#include "app/screens/common.h"
#include "app/screens/gauges.h"
#include "app/screens/pid_gauges.h"
#include "app/telemetry.h"
#include "board/s3/screens/attitude.h"

namespace ui {

static RpmScreen g_rpm;
static SpeedScreen g_speed;
static SettingsScreen g_settings;
static AttitudeHorizonScreen g_attitude;

void registerS3Screens(Navigator& nav, const disp::PanelSize& panel_size) {
    const PanelLayout layout = PanelLayout::fromSize(panel_size.width, panel_size.height);
    constexpr uint8_t kPages = 14;

    g_settings.init(layout, ScreenNavInfo{1, kPages});
    g_rpm.init(layout, ScreenNavInfo{2, kPages});
    g_speed.init(layout, ScreenNavInfo{3, kPages});
    g_attitude.init(layout, ScreenNavInfo{kPages, kPages});

    nav.addScreen(&g_settings);
    nav.addScreen(&g_rpm);
    nav.addScreen(&g_speed);
    registerPidGaugeScreens(nav, telemetryRegistry(), layout, 4, kPages);
    nav.addScreen(&g_attitude);

    TelemetryRegistry& telem = telemetryRegistry();
    telem.add(&g_rpm);
    telem.add(&g_speed);
}

}  // namespace ui
