#pragma once

#include "app/screen.h"
#include "protocol.h"

namespace ui {

/** 通用 OBD 仪表盘界面（所有板型共用逻辑） */
class ObdDashboardScreen : public Screen {
public:
    ScreenId id() const override { return ScreenId::ObdDashboard; }
    const char* title() const override { return "OBD"; }

    void onEnter() override;
    void onTick(uint32_t now_ms) override;
    void onTouch(int16_t x, int16_t y) override;

    void setTelemetry(const ObdTelemetry* telem, bool stale);

private:
    const ObdTelemetry* telem_ = nullptr;
    bool stale_ = true;
};

class SettingsScreen : public Screen {
public:
    ScreenId id() const override { return ScreenId::Settings; }
    const char* title() const override { return "Settings"; }

    void onEnter() override;
    void onTick(uint32_t now_ms) override;
};

}  // namespace ui
