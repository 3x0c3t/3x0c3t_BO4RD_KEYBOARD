#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ECRAN
// ============================================================

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SCREEN_ROTATION 2

// ============================================================
// COULEURS
// ============================================================

#define COLOR_BLACK   TFT_BLACK
#define COLOR_WHITE   TFT_WHITE
#define COLOR_RED     TFT_RED
#define COLOR_GREEN   TFT_GREEN
#define COLOR_BLUE    TFT_BLUE
#define COLOR_YELLOW  TFT_YELLOW
#define COLOR_CYAN    TFT_CYAN
#define COLOR_MAGENTA TFT_MAGENTA
#define COLOR_ORANGE  TFT_ORANGE
#define COLOR_GREY   TFT_DARKGREY

// ============================================================
// CLAVIER
// ============================================================

#define KEYBOARD_X 0
#define KEYBOARD_Y 58

#define KEYBOARD_W 320
#define KEYBOARD_H 182

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10

#define KEYBOARD_MAX_LENGTH 16

// ============================================================
// TOUCH
// ============================================================

#define TOUCH_THRESHOLD 600

// Calibration précédente
//
// Valeurs actuellement utilisées dans les logs :
// CAL0=294
// CAL1=3385
// CAL2=442
// CAL3=289
// CAL4=3387
//

#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 289
#define TOUCH_PREVIOUS_CAL_4 3387

// ============================================================
// BOUTONS CHOIX CALIBRATION
// ============================================================

#define CALIBRATION_NEW_X 20
#define CALIBRATION_NEW_Y 70
#define CALIBRATION_NEW_W 280
#define CALIBRATION_NEW_H 55

#define CALIBRATION_REUSE_X 20
#define CALIBRATION_REUSE_Y 145
#define CALIBRATION_REUSE_W 280
#define CALIBRATION_REUSE_H 55

// ============================================================
// CIBLES DE CALIBRATION
// ============================================================

#define CAL_TARGET_MARGIN 20

// ============================================================
// OBJET TFT
// ============================================================

extern TFT_eSPI tft;

#endif