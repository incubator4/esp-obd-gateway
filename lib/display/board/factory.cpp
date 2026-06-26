#include "board/factory.h"

#if defined(DISPLAY_BOARD_C6_LCD_13)

#include "panels/c6_lcd_13/panel.h"

namespace disp {

static C6Lcd13Panel g_panel;
static C6Lcd13Input g_input;

Panel* createPanel() { return &g_panel; }
Input* createInput() { return &g_input; }

void registerFeatures(FeatureRegistry& registry) {
    (void)registry;
    // C6 板暂无板载 IMU 等扩展
}

ImuAttitudeFeature* imuAttitudeFeature() { return nullptr; }

}  // namespace disp

#elif defined(DISPLAY_BOARD_S3_TOUCH_169)

#include "features/imu_attitude.h"
#include "panels/s3_touch_169/panel.h"

namespace disp {

static S3Touch169Panel g_panel;
static S3Touch169Input g_input;
static ImuAttitudeFeature g_imu;

Panel* createPanel() { return &g_panel; }
Input* createInput() { return &g_input; }

void registerFeatures(FeatureRegistry& registry) {
    registry.add(&g_imu);
}

ImuAttitudeFeature* imuAttitudeFeature() { return &g_imu; }

}  // namespace disp

#else

namespace disp {

Panel* createPanel() { return nullptr; }
Input* createInput() { return nullptr; }
void registerFeatures(FeatureRegistry&) {}
ImuAttitudeFeature* imuAttitudeFeature() { return nullptr; }

}  // namespace disp

#endif
