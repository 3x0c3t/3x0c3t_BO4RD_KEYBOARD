#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

enum TouchCalibrationState
{
    TOUCH_CALIBRATION_CHOICE,
    TOUCH_CALIBRATION_RUNNING,
    TOUCH_CALIBRATION_DONE,
    TOUCH_CALIBRATION_REUSE
};

static TouchCalibrationState calibrationState =
    TOUCH_CALIBRATION_CHOICE;

// ============================================================
// DONNEES
// ============================================================

static uint16_t calData[5];

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit()
{
    calibrationState =
        TOUCH_CALIBRATION_CHOICE;

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
// BOUTON
// ============================================================

static void drawButton(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    uint16_t color,
    const char* text
)
{
    tft.fillRoundRect(
        x,
        y,
        w,
        h,
        8,
        color
    );

    tft.drawRoundRect(
        x,
        y,
        w,
        h,
        8,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        color
    );

    tft.setTextSize(2);

    const int16_t textWidth =
        strlen(text) * 12;

    const int16_t tx =
        x +
        (w - textWidth) / 2;

    const int16_t ty =
        y +
        (h - 16) / 2;

    tft.setCursor(
        tx,
        ty
    );

    tft.print(text);
}

// ============================================================
// CHOIX CALIBRATION
// ============================================================

void touchCalibrationChoice()
{
    tft.fillScreen(
        COLOR_BLACK
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(
        25,
        25
    );

    tft.print(
        "CONFIGURATION TOUCH"
    );

    drawButton(
        CALIBRATION_NEW_X,
        CALIBRATION_NEW_Y,
        CALIBRATION_NEW_W,
        CALIBRATION_NEW_H,
        COLOR_BLUE,
        "NOUVELLE CAL."
    );

    drawButton(
        CALIBRATION_REUSE_X,
        CALIBRATION_REUSE_Y,
        CALIBRATION_REUSE_W,
        CALIBRATION_REUSE_H,
        COLOR_GREEN,
        "CAL PRECEDENTE"
    );

    calibrationState =
        TOUCH_CALIBRATION_CHOICE;

    Serial.println(
        "[TOUCH] Attente choix"
    );
}

// ============================================================
// ZONE NOUVELLE
// ============================================================

bool isNewCalibrationZone(
    uint16_t x,
    uint16_t y
)
{
    return (
        x >= CALIBRATION_NEW_X &&
        x < CALIBRATION_NEW_X +
            CALIBRATION_NEW_W &&
        y >= CALIBRATION_NEW_Y &&
        y < CALIBRATION_NEW_Y +
            CALIBRATION_NEW_H
    );
}

// ============================================================
// ZONE PRECEDENTE
// ============================================================

bool isPreviousCalibrationZone(
    uint16_t x,
    uint16_t y
)
{
    return (
        x >= CALIBRATION_REUSE_X &&
        x < CALIBRATION_REUSE_X +
            CALIBRATION_REUSE_W &&
        y >= CALIBRATION_REUSE_Y &&
        y < CALIBRATION_REUSE_Y +
            CALIBRATION_REUSE_H
    );
}

// ============================================================
// ANCIENNE CALIBRATION
// ============================================================

bool loadPreviousCalibration()
{
    calData[0] =
        TOUCH_PREVIOUS_CAL_0;

    calData[1] =
        TOUCH_PREVIOUS_CAL_1;

    calData[2] =
        TOUCH_PREVIOUS_CAL_2;

    calData[3] =
        TOUCH_PREVIOUS_CAL_3;

    calData[4] =
        TOUCH_PREVIOUS_CAL_4;

    tft.setTouch(
        calData
    );

    calibrationState =
        TOUCH_CALIBRATION_REUSE;

    Serial.println(
        "[TOUCH] Ancienne calibration chargee"
    );

    return true;
}

// ============================================================
// NOUVELLE CALIBRATION
// ============================================================

void startTouchCalibration()
{
    calibrationState =
        TOUCH_CALIBRATION_RUNNING;

    tft.fillScreen(
        COLOR_BLACK
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(
        30,
        25
    );

    tft.print(
        "CALIBRATION TOUCH"
    );

    tft.setCursor(
        30,
        55
    );

    tft.print(
        "Touchez les points"
    );

    Serial.println(
        "[TOUCH] Lancement calibration"
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

    // --------------------------------------------------------
    // CALIBRATION TFT_eSPI
    // --------------------------------------------------------

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    tft.setTouch(
        calData
    );

    calibrationState =
        TOUCH_CALIBRATION_DONE;

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
}

// ============================================================
// UPDATE
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    if (
        calibrationState !=
        TOUCH_CALIBRATION_CHOICE
    )
    {
        return (
            calibrationState ==
            TOUCH_CALIBRATION_DONE ||
            calibrationState ==
            TOUCH_CALIBRATION_REUSE
        );
    }

    Serial.print(
        "[TOUCH SELECT] X="
    );

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    // --------------------------------------------------------
    // NOUVELLE CALIBRATION
    // --------------------------------------------------------

    if (
        isNewCalibrationZone(
            x,
            y
        )
    )
    {
        Serial.println(
            "[TOUCH] CHOIX = NOUVELLE CALIBRATION"
        );

        startTouchCalibration();

        return true;
    }

    // --------------------------------------------------------
    // ANCIENNE CALIBRATION
    // --------------------------------------------------------

    if (
        isPreviousCalibrationZone(
            x,
            y
        )
    )
    {
        Serial.println(
            "[TOUCH] CHOIX = CALIBRATION PRECEDENTE"
        );

        loadPreviousCalibration();

        return true;
    }

    return false;
}

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive()
{
    return (
        calibrationState ==
            TOUCH_CALIBRATION_CHOICE
    );
}

// ============================================================
// TERMINE
// ============================================================

bool touchCalibrationFinished()
{
    return (
        calibrationState ==
            TOUCH_CALIBRATION_DONE ||
        calibrationState ==
            TOUCH_CALIBRATION_REUSE
    );
}