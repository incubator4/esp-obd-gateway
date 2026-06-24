#include "gateway_log.h"

#include <cstdarg>
#include <cstdio>

namespace {

#if GW_UART_LOG_ENABLE
HardwareSerial& uartLog() {
    static HardwareSerial port(0);
    return port;
}
#endif

void writeLine(const char* line) {
    Serial.println(line);
#if GW_UART_LOG_ENABLE
    uartLog().println(line);
#endif
}

}  // namespace

void gwLogBegin() {
    Serial.begin(115200);
#if GW_UART_LOG_ENABLE
    uartLog().begin(115200, SERIAL_8N1, GW_UART_LOG_RX_PIN, GW_UART_LOG_TX_PIN);
#endif

    // USB CDC may enumerate after boot; do not block forever.
    const uint32_t deadline = millis() + 2000;
    while (!Serial && millis() < deadline) {
        delay(10);
    }

    writeLine("");
    writeLine("[GW] serial log ready (USB CDC + FFC UART)");
}

void gwLog(const char* msg) {
    if (msg == nullptr) {
        return;
    }
    writeLine(msg);
}

void gwLogf(const char* fmt, ...) {
    if (fmt == nullptr) {
        return;
    }
    char buf[160];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    writeLine(buf);
}
