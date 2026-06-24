#pragma once

/** Waveshare ESP32-S3-Touch-LCD-1.69 — ST7789V2 240×280 + CST816T + QMI8658 */
#define DISP_S3_LCD_DC 4
#define DISP_S3_LCD_CS 5
#define DISP_S3_LCD_CLK 6
#define DISP_S3_LCD_MOSI 7
#define DISP_S3_LCD_RST 8
#define DISP_S3_LCD_BL 15

#define DISP_S3_I2C_SCL 10
#define DISP_S3_I2C_SDA 11
#define DISP_S3_TP_RST 13
#define DISP_S3_TP_INT 14
#define DISP_S3_SYS_OUT 40
#define DISP_S3_SYS_EN 41

#define DISP_S3_IMU_ADDR 0x6B
#define DISP_S3_TP_ADDR 0x15

#define DISP_S3_HOR_RES 240
#define DISP_S3_VER_RES 280

#define DISP_ESPNOW_CHANNEL 1

#include "config_link.h"
