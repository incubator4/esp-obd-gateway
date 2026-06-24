#include "core/lvgl_port.h"

#include <lvgl.h>

namespace disp {

namespace {

Panel* g_panel = nullptr;
lv_display_t* g_display = nullptr;

constexpr size_t kDrawBufPixels = 240 * 40;

alignas(4) static lv_color_t g_draw_buf1[kDrawBufPixels];
alignas(4) static lv_color_t g_draw_buf2[kDrawBufPixels];

void flushCb(lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
    if (g_panel != nullptr) {
        g_panel->flushArea(area->x1, area->y1, area->x2, area->y2,
                           reinterpret_cast<const uint16_t*>(px_map));
    }
    lv_display_flush_ready(display);
}

}  // namespace

bool lvglBegin(Panel* panel) {
    if (panel == nullptr) {
        return false;
    }

    g_panel = panel;
    lv_init();

    const PanelSize size = panel->size();
    g_display = lv_display_create(size.width, size.height);
    if (g_display == nullptr) {
        return false;
    }

    lv_display_set_user_data(g_display, panel);
    lv_display_set_flush_cb(g_display, flushCb);
    lv_display_set_buffers(g_display, g_draw_buf1, g_draw_buf2,
                             sizeof(g_draw_buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_obj_t* scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    return true;
}

void lvglTick(uint32_t elapsed_ms) { lv_tick_inc(elapsed_ms); }

void lvglHandler() { lv_timer_handler(); }

}  // namespace disp
