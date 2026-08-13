#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

/*
 * ============================================================
 * 3x0c3t BO4RD KEYBOARD v1.0
 * Configuration générale
 * ============================================================
 */

/* ============================================================
 * ECRAN
 * ============================================================ */

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  320

/*
 * Rotation TFT_eSPI
 *
 * Rotation 2 :
 * largeur  = 240
 * hauteur  = 320
 */
#define SCREEN_ROTATION 2


/* ============================================================
 * COULEURS
 * ============================================================ */

#ifndef COLOR_BLACK
#define COLOR_BLACK TFT_BLACK
#endif

#ifndef COLOR_WHITE
#define COLOR_WHITE TFT_WHITE
#endif

#ifndef COLOR_RED
#define COLOR_RED TFT_RED
#endif

#ifndef COLOR_GREEN
#define COLOR_GREEN TFT_GREEN
#endif

#ifndef COLOR_BLUE
#define COLOR_BLUE TFT_BLUE
#endif

#ifndef COLOR_YELLOW
#define COLOR_YELLOW TFT_YELLOW
#endif

#ifndef COLOR_CYAN
#define COLOR_CYAN TFT_CYAN
#endif

#ifndef COLOR_MAGENTA
#define COLOR_MAGENTA TFT_MAGENTA
#endif

#ifndef COLOR_ORANGE
#define COLOR_ORANGE 0xFD20
#endif

#ifndef COLOR_GREY
#define COLOR_GREY 0x8410
#endif

#ifndef COLOR_DARKGREY
#define COLOR_DARKGREY 0x7BEF
#endif


/* ============================================================
 * CLAVIER
 * ============================================================ */

#define KEYBOARD_X       0
#define KEYBOARD_Y       130

#define KEYBOARD_W       240
#define KEYBOARD_H       182

#define KEYBOARD_ROWS    4
#define KEYBOARD_COLS    10


/* ============================================================
 * ZONE DE SAISIE
 * ============================================================ */

#define INPUT_X          5
#define INPUT_Y          5
#define INPUT_W          230
#define INPUT_H          40


/* ============================================================
 * BOUTON SUPPRESSION
 * ============================================================ */

#define BTN_DEL_X        5
#define BTN_DEL_Y        90
#define BTN_DEL_W        110
#define BTN_DEL_H        35


/* ============================================================
 * BOUTON VALIDATION
 * ============================================================ */

#define BTN_OK_X         125
#define BTN_OK_Y         90
#define BTN_OK_W         110
#define BTN_OK_H         35


/* ============================================================
 * CALIBRATION
 * ============================================================ */

/*
 * Bouton :
 *      NOUVELLE CALIBRATION
 */

#define CALIBRATION_NEW_X  10
#define CALIBRATION_NEW_Y  115
#define CALIBRATION_NEW_W  220
#define CALIBRATION_NEW_H  70


/*
 * Bouton :
 *      CONFIG PRECEDENTE
 */

#define CALIBRATION_REUSE_X  10
#define CALIBRATION_REUSE_Y  205
#define CALIBRATION_REUSE_W  220
#define CALIBRATION_REUSE_H  70


/*
 * Anciennes constantes conservées pour compatibilité
 * avec les versions précédentes du programme.
 */

#define TOUCH_REUSE_X CALIBRATION_REUSE_X
#define TOUCH_REUSE_Y CALIBRATION_REUSE_Y
#define TOUCH_REUSE_W CALIBRATION_REUSE_W
#define TOUCH_REUSE_H CALIBRATION_REUSE_H


/* ============================================================
 * CALIBRATION PRECEDENTE
 * ============================================================ */

/*
 * Valeurs de calibration actuellement utilisées.
 *
 * Ces valeurs correspondent aux valeurs vues dans les logs :
 *
 * CAL0 = 294
 * CAL1 = 3385
 * CAL2 = 442
 * CAL3 = 289
 * CAL4 = 3387
 */

#define TOUCH_PREVIOUS_CAL_0  294
#define TOUCH_PREVIOUS_CAL_1  3385
#define TOUCH_PREVIOUS_CAL_2  442
#define TOUCH_PREVIOUS_CAL_3  289
#define TOUCH_PREVIOUS_CAL_4  3387


/* ============================================================
 * CALIBRATION
 * ============================================================ */

#define CALIBRATION_MARGIN 20


/* ============================================================
 * MODE CLAVIER
 * ============================================================ */

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC
};


/* ============================================================
 * TFT
 * ============================================================ */

extern TFT_eSPI tft;


/* ============================================================
 * FONCTIONS CLAVIER
 * ============================================================ */

void keyboardInit();

void keyboardBegin();

void keyboardDraw();

void keyboardUpdate(
    int16_t x,
    int16_t y
);

void keyboardDelete();

void keyboardValidate();

void keyboardSetMode(
    KeyboardMode mode
);

KeyboardMode keyboardGetMode();


/* ============================================================
 * FONCTIONS CALIBRATION
 * ============================================================ */

void touchCalibrationInit();

void touchCalibrationChoice();

void touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

void startTouchCalibration();

void loadPreviousCalibration();

bool touchReusePreviousCalibration();

bool isNewCalibrationZone(
    uint16_t x,
    uint16_t y
);

bool isPreviousCalibrationZone(
    uint16_t x,
    uint16_t y
);

#endif