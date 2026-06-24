#include "app/screen_store.h"

#include <Preferences.h>

namespace ui {

namespace {

constexpr const char* kNamespace = "obd_display";
constexpr const char* kScreenKey = "screen_id";

}  // namespace

bool screenStoreLoad(ScreenId& out) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, true)) {
        return false;
    }
    if (!prefs.isKey(kScreenKey)) {
        prefs.end();
        return false;
    }
    out = static_cast<ScreenId>(prefs.getUChar(kScreenKey, 0));
    prefs.end();
    return true;
}

void screenStoreSave(ScreenId id) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) {
        return;
    }
    prefs.putUChar(kScreenKey, static_cast<uint8_t>(id));
    prefs.end();
}

}  // namespace ui
