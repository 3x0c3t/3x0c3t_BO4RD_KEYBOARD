#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================


// ------------------------------------------------------------
// Ecran
// ------------------------------------------------------------

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  320

#define SCREEN_ROTATION 2


// ------------------------------------------------------------
// Calibration tactile
//
// 1 = calibration disponible au demarrage
// 0 = calibration desactivee
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Ancienne calibration
//
// Valeurs actuellement utilisees
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 289
#define TOUCH_PREVIOUS_CAL_4 3387


// ------------------------------------------------------------
// Ecran de choix calibration
// ------------------------------------------------------------

#define CALIBRATION_NEW_X       15
#define CALIBRATION_NEW_Y       110
#define CALIBRATION_NEW_W       210
#define CALIBRATION_NEW_H        70

#define CALIBRATION_REUSE_X     15
#define CALIBRATION_REUSE_Y     200
#define CALIBRATION_REUSE_W     210
#define CALIBRATION_REUSE_H      70


// ------------------------------------------------------------
// Clavier
// ------------------------------------------------------------

#define KEYBOARD_X       0
#define KEYBOARD_Y       138
#define KEYBOARD_W       240
#define KEYBOARD_H       182

#define KEYBOARD_ROWS    4
#define KEYBOARD_COLS    10


// ------------------------------------------------------------
// Champ de saisie
// ------------------------------------------------------------

#define INPUT_X          10
#define INPUT_Y          10
#define INPUT_W          220
#define INPUT_H           45


// ------------------------------------------------------------
// Bouton DEL
// ------------------------------------------------------------

#define BTN_DEL_X        10
#define BTN_DEL_Y        65
#define BTN_DEL_W        100
#define BTN_DEL_H        45


// ------------------------------------------------------------
// Bouton OK
// ------------------------------------------------------------

#define BTN_OK_X         130
#define BTN_OK_Y          65
#define BTN_OK_W         100
#define BTN_OK_H          45


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
#define COLOR_RED        0xF800
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20
#define COLOR_MAGENTA     0xF81F


// ------------------------------------------------------------
// Texte
// ------------------------------------------------------------

#define KEYBOARD_MAX_TEXT 64


#endif