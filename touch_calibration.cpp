#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// DONNEES DE CALIBRATION
// ============================================================

static uint16_t calData[5] =
{
    0,
    0,
    0,
    0,
    0
};

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit()
{
    tft.setRotation(
        SCREEN_ROTATION
    );

    Serial.println(
        "[TOUCH] Calibration Init"
    );

    Serial.print(
        "[TOUCH] Rotation = "
    );

    Serial.println(
        SCREEN_ROTATION
    );

    Serial.print(
        "[TOUCH] Resolution = "
    );

    Serial.print(
        SCREEN_WIDTH
    );

    Serial.print(
        " x "
    );

    Serial.println(
        SCREEN_HEIGHT
    );
}

// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration()
{
    Serial.println();
    Serial.println(
        "[TOUCH] ================================"
    );

    Serial.println(
        "[TOUCH] DEBUT CALIBRATION"
    );

    Serial.print(
        "[TOUCH] Rotation = "
    );

    Serial.println(
        SCREEN_ROTATION
    );

    Serial.print(
        "[TOUCH] Ecran = "
    );

    Serial.print(
        SCREEN_WIDTH
    );

    Serial.print(
        " x "
    );

    Serial.println(
        SCREEN_HEIGHT
    );

    Serial.println(
        "[TOUCH] Touchez les points affiches"
    );

    Serial.println(
        "[TOUCH] ================================"
    );

    // ========================================================
    // CALIBRATION TFT_eSPI
    // ========================================================

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    // ========================================================
    // ACTIVATION CALIBRATION
    // ========================================================

    tft.setTouch(
        calData
    );

    // ========================================================
    // RESULTATS
    // ========================================================

    Serial.println();
    Serial.println(
        "[TOUCH] Calibration terminee"
    );

    Serial.print(
        "[TOUCH] CAL0 = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "[TOUCH] CAL1 = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "[TOUCH] CAL2 = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "[TOUCH] CAL3 = "
    );

    Serial.println(
        calData[3]
    );

    Serial.print(
        "[TOUCH] CAL4 = "
    );

    Serial.println(
        calData[4]
    );

    Serial.println(
        "[TOUCH] Calibration active"
    );

    Serial.println();
}

// ============================================================
// ANCIENNE CALIBRATION
// ============================================================

bool loadPreviousCalibration()
{
    // Cette fonction reste disponible pour compatibilite.
    //
    // Pour le moment, la calibration est faite au demarrage
    // afin d'eviter toute utilisation de getTouch() avant
    // calibration.

    return false;
}

// ============================================================
// COMPATIBILITE
// ============================================================

bool touchCalibrationChoice()
{
    return true;
}

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    (void)x;
    (void)y;

    return true;
}

bool touchCalibrationActive()
{
    return false;
}

bool touchCalibrationFinished()
{
    return true;
}