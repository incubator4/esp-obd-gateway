#pragma once

#include <Arduino.h>

/** Mirror boot log to FFC UART (GPIO43 TX / GPIO44 RX) for bench adapters. */
#define GW_UART_LOG_ENABLE 1
#define GW_UART_LOG_TX_PIN 43
#define GW_UART_LOG_RX_PIN 44

#define GW_LOG_HEARTBEAT_MS 2000

void gwLogBegin();
void gwLog(const char* msg);
void gwLogf(const char* fmt, ...);
