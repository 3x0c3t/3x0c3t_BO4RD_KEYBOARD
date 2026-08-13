#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static TouchCalibrationState calibrationState =
    TOUCH_CALIBRATION_CHOICE;

static uint16_t calData[5] =
{
    0,
    0,
    0,
    0,
    0
};

static uint8_t calibrationPoint = 0;

// ============================================================
// CHARGER CONFIG PRECEDENTE
// ============================================================

bool loadPreviousCalibration()
{
    calData[0] = TOUCH_PREVIOUS_CAL_0;
    calData[1] = TOUCH_PREVIOUS_CAL_1;
    calData[2] = TOUCH_PREVIOUS_CAL_2;
    calData[3] = TOUCH_PREVIOUS_CAL_3;
    calData[4] = TOUCH_PREVIOUS_CAL_4;

    tft.setTouch(calData);

    Serial.println(
        "[TOUCH] Configuration precedente chargee"
    );

    Serial.print("[TOUCH] CAL0=");
    Serial.println(calData[0]);

    Serial.print("[TOUCH] CAL1=");
    Serial.println(calData[1]);

    Serial.print("[TOUCH] CAL2=");
    Serial.println(calData[2]);

    Serial.print("[TOUCH] CAL3=");
    Serial.println(calData[3]);

    Serial.print("[TOUCH] CAL4=");
    Serial.println(calData[4]);

    calibrationState =
        TOUCH_CALIBRATION_REUSE;

    return true;
}

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit()
{
    calibrationState =
        TOUCH_CALIBRATION_CHOICE;

    calibrationPoint = 0;

    tft.setRotation(
        SCREEN_ROTATION
    );

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );
}

// ============================================================
// ECRAN DE CHOIX
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

    int16_t textWidth =
        strlen(text) * 12;

    int16_t tx =
        x + (w - textWidth) / 2;

    int16_t ty =
        y + (h - 16) / 2;

    tft.setCursor(
        tx,
        ty
    );

    tft.print(text);
}

static void drawCalibrationChoiceScreen()
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
        35,
        20
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
        "NOUVELLE CALIBRATION"
    );

    drawButton(
        CALIBRATION_REUSE_X,
        CALIBRATION_REUSE_Y,
        CALIBRATION_REUSE_W,
        CALIBRATION_REUSE_H,
        COLOR_GREEN,
        "CONFIG PRECEDENTE"
    );
}

// ============================================================
// CHOIX
// ============================================================

bool touchCalibrationChoice()
{
    drawCalibrationChoiceScreen();

    calibrationState =
        TOUCH_CALIBRATION_CHOICE;

    Serial.println(
        "[TOUCH] Attente choix utilisateur"
    );

    return false;
}

// ============================================================
// ZONES
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
// DEMARRER CALIBRATION
// ============================================================

void startTouchCalibration()
{
    calibrationState =
        TOUCH_CALIBRATION_RUNNING;

    calibrationPoint = 0;

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
        20
    );

    tft.print(
        "CALIBRATION TOUCH"
    );

    Serial.println(
        "[TOUCH] Lancement calibration"
    );

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
        calibrationState ==
        TOUCH_CALIBRATION_CHOICE
    )
    {
        Serial.print(
            "[TOUCH SELECT] X="
        );
        Serial.print(x);

        Serial.print(
            " Y="
        );
        Serial.println(y);

        if (isNewCalibrationZone(x, y))
        {
            Serial.println(
                "[TOUCH] CHOIX = NOUVELLE CALIBRATION"
            );

            startTouchCalibration();

            return true;
        }

        if (isPreviousCalibrationZone(x, y))
        {
            Serial.println(
                "[TOUCH] CHOIX = CONFIG PRECEDENTE"
            );

            if (loadPreviousCalibration())
            {
                Serial.println(
                    "[TOUCH] Passage au clavier"
                );

                return true;
            }
        }

        return false;
    }

    return (
        calibrationState ==
        TOUCH_CALIBRATION_DONE ||
        calibrationState ==
        TOUCH_CALIBRATION_REUSE
    );
}

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive()
{
    return (
        calibrationState ==
        TOUCH_CALIBRATION_RUNNING ||
        calibrationState ==
        TOUCH_CALIBRATION_CHOICE
    );
}

bool touchCalibrationFinished()
{
    return (
        calibrationState ==
        TOUCH_CALIBRATION_DONE ||
        calibrationState ==
        TOUCH_CALIBRATION_REUSE
    );
}