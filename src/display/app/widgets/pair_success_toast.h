#pragma once

#include <lvgl.h>

#include <cstdint>

namespace ui {

/** Full-screen overlay shown briefly after gateway pairing succeeds. */
class PairSuccessToast {
public:
    void show(const uint8_t gateway_mac[6]);
    void tick(uint32_t now_ms = 0);
    bool isActive() const { return overlay_ != nullptr; }

private:
    void dismiss();

    static void animArcCb(void* obj, int32_t value);
    static void animOpaCb(void* obj, int32_t value);

    lv_obj_t* overlay_ = nullptr;
    lv_obj_t* card_ = nullptr;
    uint32_t shown_at_ms_ = 0;
};

PairSuccessToast& pairSuccessToast();

}  // namespace ui
