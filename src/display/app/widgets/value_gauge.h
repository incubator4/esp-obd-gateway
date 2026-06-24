#pragma once

#include "app/layout.h"

#include <lvgl.h>

#include <cstdint>

namespace ui {

/** 弧形数值仪表：标题 + 弧 + 数值 + 单位，跨板型复用 */
class ValueGauge {
public:
    struct Config {
        const char* title = "";
        const char* unit = "";
        int32_t min_value = 0;
        int32_t max_value = 100;
        uint32_t arc_color = 0x00D4AA;
        uint32_t stale_color = 0x555555;
        bool compact = false;
    };

    void create(lv_obj_t* parent, const GaugeSlot& slot, const Config& config);
    void destroy();
    void setValue(int32_t value, bool stale);

private:
    void refreshLabels(int32_t value, bool stale);

    lv_obj_t* root_ = nullptr;
    lv_obj_t* arc_ = nullptr;
    lv_obj_t* value_label_ = nullptr;
    lv_obj_t* unit_label_ = nullptr;
    Config config_{};
    int32_t last_value_ = -1;
    bool last_stale_ = true;
};

}  // namespace ui
