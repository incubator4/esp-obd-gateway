#pragma once

#include <cstddef>
#include <cstdint>

namespace disp {

enum class FeatureId : uint8_t {
    ImuAttitude = 1,
};

/** 板载扩展能力（IMU、RTC 等） */
class Feature {
public:
    virtual ~Feature() = default;

    virtual FeatureId id() const = 0;
    virtual bool begin() = 0;
    virtual void update() = 0;
    virtual bool available() const = 0;
};

class FeatureRegistry {
public:
    static constexpr size_t kMaxFeatures = 4;

    void add(Feature* feature);
    Feature* get(FeatureId id);

    void beginAll();
    void updateAll();

private:
    Feature* items_[kMaxFeatures]{};
    size_t count_ = 0;
};

}  // namespace disp
