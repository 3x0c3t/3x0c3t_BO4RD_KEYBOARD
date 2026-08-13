#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================

// ------------------------------------------------------------
// Écran
// ------------------------------------------------------------

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

#define SCREEN_ROTATION 2

// ------------------------------------------------------------
// Calibration tactile
//
// 1 = calibration à chaque démarrage
// 0 = pas de calibration
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1

// ------------------------------------------------------------
// Zone clavier
// ------------------------------------------------------------

#define KEYBOARD_X 0
#define KEYBOARD_Y 138
#define KEYBOARD_W 240
#define KEYBOARD_H 182

// ------------------------------------------------------------
// Couleurs
// ------------------------------------------------------------

#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_GREY        0x7BEF
#define COLOR_DARK_GREY   0x39E7
#define COLOR_BLUE        0x001F
#define COLOR_CYAN        0x07FF
#define COLOR_GREEN       0x07E0
#define COLOR_RED         0xF800
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20

// ------------------------------------------------------------
// Texte
// ------------------------------------------------------------

#define KEYBOARD_MAX_TEXT 64

#endif