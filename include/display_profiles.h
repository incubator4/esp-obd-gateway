#pragma once

/**
 * Waveshare display profiles — one macro per chip family × screen size.
 * Touch vs non-touch is detected at runtime on I2C (see panels directory).
 *
 * Build: -D DISPLAY_PROFILE_C6_13 | C6_147 | S3_169 -include display_profiles.h
 */

#if defined(DISPLAY_PROFILE_C6_13)

#define DISPLAY_PROFILE_NAME "C6-1.3"
#define DISPLAY_CHIP_C6 1

/** ESP32-C6-LCD-1.3 — ST7789V2 240×240, BOOT only (no touch bus) */
#define DISP_C6_LCD_MOSI 6
#define DISP_C6_LCD_SCLK 7
#define DISP_C6_LCD_CS 14
#define DISP_C6_LCD_DC 15
#define DISP_C6_LCD_RST 21
#define DISP_C6_LCD_BL 22

#define DISP_C6_HOR_RES 240
#define DISP_C6_VER_RES 240

#define DISP_C6_BOOT_PIN 9

#elif defined(DISPLAY_PROFILE_C6_147)

#define DISPLAY_PROFILE_NAME "C6-1.47"
#define DISPLAY_CHIP_C6 1

/** ESP32-C6-LCD/Touch-LCD-1.47 — JD9853 172×320; touch AXS5106L @ 0x63 if present */
#define DISP_C6_LCD_MOSI 2
#define DISP_C6_LCD_MISO 3
#define DISP_C6_LCD_SCLK 1
#define DISP_C6_LCD_CS 14
#define DISP_C6_LCD_DC 15
#define DISP_C6_LCD_RST 22
#define DISP_C6_LCD_BL 23
#define DISP_C6_SD_CS 4

#define DISP_C6_I2C_SDA 18
#define DISP_C6_I2C_SCL 19
#define DISP_C6_TP_RST 20
#define DISP_C6_TP_INT 21
#define DISP_C6_TP_ADDR 0x63

#define DISP_C6_HOR_RES 172
#define DISP_C6_VER_RES 320

#define DISP_C6_LCD_COL_OFFSET1 34
#define DISP_C6_LCD_COL_OFFSET2 34

#define DISP_C6_BOOT_PIN 9

#elif defined(DISPLAY_PROFILE_S3_169)

#define DISPLAY_PROFILE_NAME "S3-1.69"
#define DISPLAY_CHIP_S3 1

/** ESP32-S3-LCD/Touch-LCD-1.69 — ST7789V2 240×280; touch CST816T @ 0x15 if present */
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

#else

#error "Define DISPLAY_PROFILE_C6_13, DISPLAY_PROFILE_C6_147, or DISPLAY_PROFILE_S3_169"

#endif

#define DISP_ESPNOW_CHANNEL 1

#include "config_link.h"
