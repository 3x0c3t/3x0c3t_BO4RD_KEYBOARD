#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================


// ------------------------------------------------------------
// Écran
// ------------------------------------------------------------

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  320

#define SCREEN_ROTATION 2


// ------------------------------------------------------------
// Calibration tactile
//
// 1 = calibration à chaque démarrage
// 0 = utilisation des valeurs précédentes
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Zone clavier
// ------------------------------------------------------------

#define KEYBOARD_X 0
#define KEYBOARD_Y 138
#define KEYBOARD_W 240
#define KEYBOARD_H 182

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10


// ------------------------------------------------------------
// Champ de saisie
// ------------------------------------------------------------

#define INPUT_X 5
#define INPUT_Y 8
#define INPUT_W 230
#define INPUT_H 40


// ------------------------------------------------------------
// Bouton DEL
// ------------------------------------------------------------

#define BTN_DEL_X 5
#define BTN_DEL_Y 50
#define BTN_DEL_W 110
#define BTN_DEL_H 40


// ------------------------------------------------------------
// Bouton OK
// ------------------------------------------------------------

#define BTN_OK_X 120
#define BTN_OK_Y 50
#define BTN_OK_W 115
#define BTN_OK_H 40


// ------------------------------------------------------------
// Zone de réutilisation calibration
// ------------------------------------------------------------

#define TOUCH_REUSE_X 20
#define TOUCH_REUSE_Y 270
#define TOUCH_REUSE_W 200
#define TOUCH_REUSE_H 35


// ------------------------------------------------------------
// Valeurs de calibration précédentes
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 3380
#define TOUCH_PREVIOUS_CAL_4 2


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