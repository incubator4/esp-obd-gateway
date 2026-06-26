#include "board/factory.h"

#if defined(DISPLAY_PROFILE_C6_13)

#include "panels/c6_13/panel.h"

namespace disp {

static C613Panel g_panel;
static C613Input g_input;

Panel* createPanel() { return &g_panel; }
Input* createInput() { return &g_input; }

void registerFeatures(FeatureRegistry& registry) {
    (void)registry;
}

ImuAttitudeFeature* imuAttitudeFeature() { return nullptr; }

}  // namespace disp

#elif defined(DISPLAY_PROFILE_C6_147)

#include "panels/c6_147/panel.h"

namespace disp {

static C6147Panel g_panel;
static C6147Input g_input;

Panel* createPanel() { return &g_panel; }
Input* createInput() { return &g_input; }

void registerFeatures(FeatureRegistry& registry) {
    (void)registry;
}

ImuAttitudeFeature* imuAttitudeFeature() { return nullptr; }

}  // namespace disp

#elif defined(DISPLAY_PROFILE_S3_169)

#include "features/imu_attitude.h"
#include "panels/s3_169/panel.h"

namespace disp {

static S3169Panel g_panel;
static S3169Input g_input;
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
