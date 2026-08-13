#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================
// 3x0c3t BO4RD - KEYBOARD
// ESP8266 + ILI9341 240x320 + XPT2046
// Rotation 2
// ============================================================

// ------------------------------------------------------------
// TFT ILI9341
// ------------------------------------------------------------

#define TFT_CS      15    // D8
#define TFT_DC       2    // D4
#define TFT_RST     16    // D0

// SPI matériel ESP8266
#define SPI_SCK     14    // D5
#define SPI_MISO    12    // D6
#define SPI_MOSI    13    // D7

// ------------------------------------------------------------
// XPT2046
// ------------------------------------------------------------

#define TOUCH_CS     0    // D3
#define TOUCH_IRQ    5    // D1

// Le XPT2046 partage le SPI avec le TFT.
// Ne PAS utiliser SPI.begin(SCK,MISO,MOSI,CS) sur ESP8266.

// ------------------------------------------------------------
// Ecran
// ------------------------------------------------------------

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

#define TFT_ROTATION   2

// ------------------------------------------------------------
// Zone de texte
// ------------------------------------------------------------

#define TEXT_AREA_X     5
#define TEXT_AREA_Y     5
#define TEXT_AREA_W   310
#define TEXT_AREA_H    42

#define TEXT_CURSOR_X  10
#define TEXT_CURSOR_Y  14

#define MAX_TEXT_LENGTH 64

// ------------------------------------------------------------
// Clavier
// ------------------------------------------------------------

#define KEYBOARD_X       5
#define KEYBOARD_Y      53
#define KEYBOARD_W     310
#define KEYBOARD_H     182

#define KEY_GAP           4
#define KEY_RADIUS        5

// ------------------------------------------------------------
// Couleurs RGB565
// ------------------------------------------------------------

#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_CYAN        0x07FF
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20
#define COLOR_GRAY        0x8410
#define COLOR_DARKGRAY    0x4208
#define COLOR_LIGHTGRAY   0xC618

// ------------------------------------------------------------
// Calibration tactile
//
// Ces valeurs sont les valeurs RAW XPT2046.
// Elles peuvent être ajustées si nécessaire.
//
// Pour rotation 2, le mapping est effectué dans
// touch_calibration.cpp.
// ------------------------------------------------------------

#define TOUCH_MIN_X       200
#define TOUCH_MAX_X      3900

#define TOUCH_MIN_Y       200
#define TOUCH_MAX_Y      3900

// Pression minimale considérée comme un appui
#define TOUCH_PRESSURE_MIN 300

// Anti-rebond
#define TOUCH_DEBOUNCE_MS 120

#endif