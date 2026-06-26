#pragma once

#include "core/feature.h"
#include "core/input.h"
#include "core/panel.h"

namespace disp {

class ImuAttitudeFeature;

Panel* createPanel();
Input* createInput();
void registerFeatures(FeatureRegistry& registry);
ImuAttitudeFeature* imuAttitudeFeature();

}  // namespace disp
