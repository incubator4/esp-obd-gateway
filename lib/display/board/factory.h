#pragma once

#include "core/feature.h"
#include "core/input.h"
#include "core/panel.h"

namespace disp {

Panel* createPanel();
Input* createInput();
void registerFeatures(FeatureRegistry& registry);

}  // namespace disp
