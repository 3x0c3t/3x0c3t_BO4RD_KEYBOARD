#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

#include <Arduino.h>

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// ============================================================
// CLAVIER
// ============================================================

#define KEYBOARD_X 0
#define KEYBOARD_Y 125

#define KEYBOARD_W 240
#define KEYBOARD_H 182

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10

#define KEYBOARD_MAX_LENGTH 32

// ============================================================
// ZONE TEXTE
// ============================================================

#define INPUT_X 5
#define INPUT_Y 5
#define INPUT_W 230
#define INPUT_H 35

// ============================================================
// BOUTON DEL
// ============================================================

#define BTN_DEL_X 5
#define BTN_DEL_Y 90
#define BTN_DEL_W 70
#define BTN_DEL_H 30

// ============================================================
// BOUTON OK
// ============================================================

#define BTN_OK_X 165
#define BTN_OK_Y 90
#define BTN_OK_W 70
#define BTN_OK_H 30

// ============================================================
// CALIBRATION
// ============================================================

// Valeurs de calibration précédentes
#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 289
#define TOUCH_PREVIOUS_CAL_4 3387

// ============================================================
// CHOIX CALIBRATION
// ============================================================

#define CALIBRATION_NEW_X 10
#define CALIBRATION_NEW_Y 90
#define CALIBRATION_NEW_W 220
#define CALIBRATION_NEW_H 55

#define CALIBRATION_REUSE_X 10
#define CALIBRATION_REUSE_Y 165
#define CALIBRATION_REUSE_W 220
#define CALIBRATION_REUSE_H 55

// ============================================================
// COULEURS
// ============================================================

#define COLOR_MAGENTA TFT_MAGENTA

#endif