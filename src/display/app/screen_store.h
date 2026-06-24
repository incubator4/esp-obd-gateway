#pragma once

#include "app/screen.h"

namespace ui {

/** Load last screen from NVS. Returns false if none saved or NVS unavailable. */
bool screenStoreLoad(ScreenId& out);

/** Persist current screen for next boot. */
void screenStoreSave(ScreenId id);

}  // namespace ui
