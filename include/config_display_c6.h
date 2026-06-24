#pragma once

/** Waveshare ESP32-C6-LCD-1.3 — ST7789V2, 240×240, SPI（引脚以 Waveshare 示例为准） */
#define DISP_C6_LCD_MOSI 6
#define DISP_C6_LCD_SCLK 7
#define DISP_C6_LCD_CS 14
#define DISP_C6_LCD_DC 15
#define DISP_C6_LCD_RST 21
#define DISP_C6_LCD_BL 22

#define DISP_C6_HOR_RES 240
#define DISP_C6_VER_RES 240

/** BOOT 键：ESP32-C6 板载 BOOT 接 GPIO9（非 ESP32/S3 的 GPIO0） */
#define DISP_C6_BOOT_PIN 9

#define DISP_ESPNOW_CHANNEL 1
