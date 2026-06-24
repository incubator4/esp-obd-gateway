#pragma once

#include "app/layout.h"
#include "protocol.h"

#include <lvgl.h>

#include <cstdint>

namespace ui {

struct ScreenNavInfo {
    uint8_t index = 1;
    uint8_t total = 1;
};

void styleScreenRoot(lv_obj_t* root);
lv_obj_t* createHeader(lv_obj_t* parent, const char* title, uint16_t header_h);
lv_obj_t* createFooter(lv_obj_t* parent, const PanelLayout& layout,
                       const ScreenNavInfo& nav, bool boot_only);
void updateFooterStatus(lv_obj_t* footer_label, const ObdTelemetry* telem, bool stale,
                        const ScreenNavInfo& nav, bool boot_only);

}  // namespace ui
