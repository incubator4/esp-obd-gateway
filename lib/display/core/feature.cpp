#include "core/feature.h"

namespace disp {

void FeatureRegistry::add(Feature* feature) {
    if (feature == nullptr || count_ >= kMaxFeatures) {
        return;
    }
    items_[count_++] = feature;
}

Feature* FeatureRegistry::get(FeatureId id) {
    for (size_t i = 0; i < count_; ++i) {
        if (items_[i] != nullptr && items_[i]->id() == id) {
            return items_[i];
        }
    }
    return nullptr;
}

void FeatureRegistry::beginAll() {
    for (size_t i = 0; i < count_; ++i) {
        if (items_[i] != nullptr) {
            items_[i]->begin();
        }
    }
}

void FeatureRegistry::updateAll() {
    for (size_t i = 0; i < count_; ++i) {
        if (items_[i] != nullptr && items_[i]->available()) {
            items_[i]->update();
        }
    }
}

}  // namespace disp
