#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================


// ------------------------------------------------------------
// Ecran
// ------------------------------------------------------------

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

#define SCREEN_ROTATION 2


// ------------------------------------------------------------
// Calibration tactile
// ------------------------------------------------------------
//
// 1 = afficher le choix de calibration au démarrage
// 0 = utiliser directement les valeurs précédentes
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Calibration précédente
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 289
#define TOUCH_PREVIOUS_CAL_4 3387


// ------------------------------------------------------------
// Ecran de choix calibration
// ------------------------------------------------------------

// Bouton NOUVELLE CALIBRATION

#define CALIBRATION_NEW_X 20
#define CALIBRATION_NEW_Y 90
#define CALIBRATION_NEW_W 200
#define CALIBRATION_NEW_H 60


// Bouton CONFIGURATION PRECEDENTE

#define CALIBRATION_REUSE_X 20
#define CALIBRATION_REUSE_Y 170
#define CALIBRATION_REUSE_W 200
#define CALIBRATION_REUSE_H 60


// ------------------------------------------------------------
// Clavier
// ------------------------------------------------------------

#define KEYBOARD_X 0
#define KEYBOARD_Y 138
#define KEYBOARD_W 240
#define KEYBOARD_HEIGHT 182

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10


// ------------------------------------------------------------
// Champ de saisie
// ------------------------------------------------------------

#define INPUT_X 0
#define INPUT_Y 105
#define INPUT_W 240
#define INPUT_H 30


// ------------------------------------------------------------
// Bouton DEL
// ------------------------------------------------------------

#define BTN_DEL_X 0
#define BTN_DEL_Y 65
#define BTN_DEL_W 80
#define BTN_DEL_H 35


// ------------------------------------------------------------
// Bouton OK
// ------------------------------------------------------------

#define BTN_OK_X 160
#define BTN_OK_Y 65
#define BTN_OK_W 80
#define BTN_OK_H 35


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