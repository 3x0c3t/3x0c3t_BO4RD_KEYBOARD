#ifndef BO4RD_KEYBOARD_CONFIG_H
#define BO4RD_KEYBOARD_CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ECRAN
// ============================================================

#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      240
#define SCREEN_ROTATION    2

// ============================================================
// COULEURS
// ============================================================

#define COLOR_BLACK        TFT_BLACK
#define COLOR_WHITE        TFT_WHITE
#define COLOR_RED          TFT_RED
#define COLOR_GREEN        TFT_GREEN
#define COLOR_BLUE         TFT_BLUE
#define COLOR_YELLOW       TFT_YELLOW
#define COLOR_CYAN         TFT_CYAN
#define COLOR_MAGENTA      TFT_MAGENTA
#define COLOR_GREY         TFT_DARKGREY

// ============================================================
// TOUCH
// ============================================================

#define TOUCH_THRESHOLD    600

// ============================================================
// CALIBRATION PRECEDENTE
// ============================================================

#define TOUCH_PREVIOUS_CAL_0  294
#define TOUCH_PREVIOUS_CAL_1  3385
#define TOUCH_PREVIOUS_CAL_2  442
#define TOUCH_PREVIOUS_CAL_3  289
#define TOUCH_PREVIOUS_CAL_4  3387

// ============================================================
// ECRAN CHOIX CALIBRATION
// ============================================================

#define CALIBRATION_NEW_X     20
#define CALIBRATION_NEW_Y     75
#define CALIBRATION_NEW_W     280
#define CALIBRATION_NEW_H     55

#define CALIBRATION_REUSE_X   20
#define CALIBRATION_REUSE_Y   145
#define CALIBRATION_REUSE_W   280
#define CALIBRATION_REUSE_H   55

// ============================================================
// CLAVIER
// ============================================================

#define KEYBOARD_X            0
#define KEYBOARD_Y            58
#define KEYBOARD_W            320
#define KEYBOARD_HEIGHT       182

#define KEYBOARD_ROWS         4
#define KEYBOARD_COLS         10

#define KEYBOARD_MAX_LENGTH   32

// ============================================================
// ZONE TEXTE
// ============================================================

#define INPUT_X               10
#define INPUT_Y               5
#define INPUT_W               220
#define INPUT_H               42

// ============================================================
// BOUTON DELETE
// ============================================================

#define BTN_DEL_X             235
#define BTN_DEL_Y             5
#define BTN_DEL_W             75
#define BTN_DEL_H             42

// ============================================================
// BOUTON OK
// ============================================================

#define BTN_OK_X              235
#define BTN_OK_Y              193
#define BTN_OK_W              75
#define BTN_OK_H              42

// ============================================================
// AFFICHAGE
// ============================================================

#define KEYBOARD_TEXT_SIZE    2
#define KEYBOARD_BORDER       2

#endif