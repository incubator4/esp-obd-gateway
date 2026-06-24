#include "bus/can.h"

#include <Arduino.h>

namespace obd {

ObdCan::~ObdCan() { end(); }

twai_timing_config_t ObdCan::timingFor(ObdBitrate br) {
    switch (br) {
        case ObdBitrate::k125K:
            return TWAI_TIMING_CONFIG_125KBITS();
        case ObdBitrate::k250K:
            return TWAI_TIMING_CONFIG_250KBITS();
        case ObdBitrate::k500K:
        default:
            return TWAI_TIMING_CONFIG_500KBITS();
    }
}

void ObdCan::configureRsPin() const {
    const int rs = cfg_.pins.rs;
    if (rs < 0) {
        return;
    }
    pinMode(rs, OUTPUT);
    const int level = cfg_.pins.rs_active_high ? HIGH : LOW;
    digitalWrite(rs, level);
}

bool ObdCan::begin(const ObdCanConfig& cfg) {
    end();
    cfg_ = cfg;
    configureRsPin();

    twai_general_config_t general = TWAI_GENERAL_CONFIG_DEFAULT(
        static_cast<gpio_num_t>(cfg_.pins.tx),
        static_cast<gpio_num_t>(cfg_.pins.rx),
        TWAI_MODE_NORMAL);
    general.rx_queue_len = 16;
    general.tx_queue_len = 8;

    const twai_timing_config_t timing = timingFor(cfg_.bitrate);
    const twai_filter_config_t filter = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&general, &timing, &filter) != ESP_OK) {
        return false;
    }
    if (twai_start() != ESP_OK) {
        twai_driver_uninstall();
        return false;
    }

    const uint32_t alerts = TWAI_ALERT_BUS_OFF | TWAI_ALERT_ERR_PASS |
                            TWAI_ALERT_BUS_RECOVERED | TWAI_ALERT_BUS_ERROR;
    twai_reconfigure_alerts(alerts, nullptr);

    running_ = true;
    return true;
}

void ObdCan::end() {
    if (!running_) {
        return;
    }
    twai_stop();
    twai_driver_uninstall();
    running_ = false;
}

bool ObdCan::send(const twai_message_t& msg, uint32_t timeout_ms) {
    if (!running_) {
        return false;
    }
    return twai_transmit(&msg, pdMS_TO_TICKS(timeout_ms)) == ESP_OK;
}

bool ObdCan::recv(twai_message_t& msg, uint32_t timeout_ms) {
    if (!running_) {
        return false;
    }
    return twai_receive(&msg, pdMS_TO_TICKS(timeout_ms)) == ESP_OK;
}

bool ObdCan::recover(uint32_t timeout_ms) {
    if (!running_) {
        return false;
    }

    twai_status_info_t info{};
    if (twai_get_status_info(&info) != ESP_OK) {
        return false;
    }
    if (info.state != TWAI_STATE_BUS_OFF) {
        return true;
    }

    if (twai_initiate_recovery() != ESP_OK) {
        return false;
    }

    const uint32_t deadline = millis() + timeout_ms;
    while (millis() < deadline) {
        if (twai_get_status_info(&info) == ESP_OK &&
            info.state != TWAI_STATE_BUS_OFF) {
            return twai_start() == ESP_OK;
        }
        delay(10);
    }
    return false;
}

twai_status_info_t ObdCan::status() const {
    twai_status_info_t info{};
    if (running_) {
        twai_get_status_info(&info);
    }
    return info;
}

}  // namespace obd
