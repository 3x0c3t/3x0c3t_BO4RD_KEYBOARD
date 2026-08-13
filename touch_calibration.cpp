#include "touch_calibration.h"
#include "config.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;


// ============================================================
// DONNÉES DE CALIBRATION
// ============================================================

uint16_t calData[5] =
{
    TOUCH_PREVIOUS_CAL_0,
    TOUCH_PREVIOUS_CAL_1,
    TOUCH_PREVIOUS_CAL_2,
    TOUCH_PREVIOUS_CAL_3,
    TOUCH_PREVIOUS_CAL_4
};


// ============================================================
// ÉTAT
// ============================================================

static bool calibrationChoiceActive = false;


// ============================================================
// LOG SIMPLE
// ============================================================

static void logCalibration(const char *message)
{
    Serial.print("[TOUCH] ");
    Serial.println(message);
}


// ============================================================
// CHARGEMENT ANCIENNE CALIBRATION
// ============================================================

void loadPreviousCalibration()
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
    Serial.print(calData[0]);

    Serial.print(" CAL1=");
    Serial.print(calData[1]);

    Serial.print(" CAL2=");
    Serial.print(calData[2]);

    Serial.print(" CAL3=");
    Serial.print(calData[3]);

    Serial.print(" CAL4=");
    Serial.println(calData[4]);
}


// ============================================================
// ÉCRAN CHOIX CALIBRATION
// ============================================================

void drawCalibrationChoiceScreen()
{
    tft.fillScreen(COLOR_BLACK);

    // --------------------------------------------------------
    // TITRE
    // --------------------------------------------------------

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);

    tft.drawString(
        "CALIBRATION TACTILE",
        SCREEN_WIDTH / 2,
        30,
        2
    );


    // --------------------------------------------------------
    // DESCRIPTION
    // --------------------------------------------------------

    tft.setTextColor(COLOR_GREY, COLOR_BLACK);

    tft.drawString(
        "Choisissez une option",
        SCREEN_WIDTH / 2,
        65,
        2
    );


    // ========================================================
    // BOUTON NOUVELLE CALIBRATION
    // ========================================================

    tft.fillRoundRect(
        CALIBRATION_NEW_X,
        CALIBRATION_NEW_Y,
        CALIBRATION_NEW_W,
        CALIBRATION_NEW_H,
        8,
        COLOR_BLUE
    );

    tft.drawRoundRect(
        CALIBRATION_NEW_X,
        CALIBRATION_NEW_Y,
        CALIBRATION_NEW_W,
        CALIBRATION_NEW_H,
        8,
        COLOR_WHITE
    );

    tft.setTextColor(COLOR_WHITE, COLOR_BLUE);

    tft.drawString(
        "NOUVELLE",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 25,
        2
    );

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 48,
        2
    );


    // ========================================================
    // BOUTON CONFIGURATION PRÉCÉDENTE
    // ========================================================

    tft.fillRoundRect(
        CALIBRATION_REUSE_X,
        CALIBRATION_REUSE_Y,
        CALIBRATION_REUSE_W,
        CALIBRATION_REUSE_H,
        8,
        COLOR_GREEN
    );

    tft.drawRoundRect(
        CALIBRATION_REUSE_X,
        CALIBRATION_REUSE_Y,
        CALIBRATION_REUSE_W,
        CALIBRATION_REUSE_H,
        8,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_GREEN
    );

    tft.drawString(
        "CONFIGURATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 23,
        2
    );

    tft.drawString(
        "PRECEDENTE",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 47,
        2
    );


    // --------------------------------------------------------
    // INFORMATION
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_RED,
        COLOR_BLACK
    );

    tft.drawString(
        "La calibration peut etre",
        SCREEN_WIDTH / 2,
        285,
        2
    );

    tft.drawString(
        "desactivee dans config.h",
        SCREEN_WIDTH / 2,
        305,
        2
    );
}


// ============================================================
// ZONE NOUVELLE CALIBRATION
// ============================================================

bool isNewCalibrationZone(
    uint16_t x,
    uint16_t y
)
{
    return
        x >= CALIBRATION_NEW_X &&
        x < CALIBRATION_NEW_X + CALIBRATION_NEW_W &&
        y >= CALIBRATION_NEW_Y &&
        y < CALIBRATION_NEW_Y + CALIBRATION_NEW_H;
}


// ============================================================
// ZONE CONFIGURATION PRÉCÉDENTE
// ============================================================

bool isPreviousCalibrationZone(
    uint16_t x,
    uint16_t y
)
{
    return
        x >= CALIBRATION_REUSE_X &&
        x < CALIBRATION_REUSE_X + CALIBRATION_REUSE_W &&
        y >= CALIBRATION_REUSE_Y &&
        y < CALIBRATION_REUSE_Y + CALIBRATION_REUSE_H;
}


// ============================================================
// ÉCRAN DE CALIBRATION
// ============================================================

static void drawCalibrationScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        25,
        2
    );

    tft.setTextColor(
        COLOR_GREY,
        COLOR_BLACK
    );

    tft.drawString(
        "Touchez les cibles",
        SCREEN_WIDTH / 2,
        50,
        2
    );

    tft.drawString(
        "dans les 4 angles",
        SCREEN_WIDTH / 2,
        72,
        2
    );
}


// ============================================================
// LANCEMENT NOUVELLE CALIBRATION
// ============================================================

void startTouchCalibration()
{
    Serial.println(
        "[TOUCH] Lancement calibration"
    );

    drawCalibrationScreen();

    delay(500);

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    Serial.println(
        "=== CALIBRATION TERMINE ==="
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

    tft.setTouch(calData);
}


// ============================================================
// INITIALISATION CALIBRATION
// ============================================================

void touchCalibrationInit()
{
    Serial.println(
        "[TOUCH] Init TFT_eSPI"
    );

    Serial.print(
        "[TOUCH] Rotation="
    );

    Serial.println(
        SCREEN_ROTATION
    );

    // --------------------------------------------------------
    // Calibration désactivée
    // --------------------------------------------------------

    if (TOUCH_CALIBRATION == 0)
    {
        Serial.println(
            "[TOUCH] Calibration DESACTIVEE"
        );

        loadPreviousCalibration();

        calibrationChoiceActive = false;

        return;
    }


    // --------------------------------------------------------
    // Calibration activée
    // --------------------------------------------------------

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    loadPreviousCalibration();

    drawCalibrationChoiceScreen();

    calibrationChoiceActive = true;

    Serial.println(
        "[TOUCH] Attente choix utilisateur"
    );
}


// ============================================================
// TRAITEMENT DU CHOIX
// ============================================================
//
// Retourne :
//
// false = aucun choix
// true  = choix effectué
//
// choice =
// 0 = aucune action
// 1 = nouvelle calibration
// 2 = ancienne configuration
// ============================================================

bool touchCalibrationChoice(
    uint16_t x,
    uint16_t y,
    uint8_t &choice
)
{
    choice = 0;

    if (!calibrationChoiceActive)
    {
        return false;
    }


    // --------------------------------------------------------
    // NOUVELLE CALIBRATION
    // --------------------------------------------------------

    if (isNewCalibrationZone(x, y))
    {
        Serial.print(
            "[TOUCH SELECT] X="
        );

        Serial.print(x);

        Serial.print(
            " Y="
        );

        Serial.println(y);

        Serial.println(
            "[TOUCH] CHOIX = NOUVELLE CALIBRATION"
        );

        calibrationChoiceActive = false;

        choice = 1;

        delay(200);

        startTouchCalibration();

        return true;
    }


    // --------------------------------------------------------
    // CONFIGURATION PRÉCÉDENTE
    // --------------------------------------------------------

    if (isPreviousCalibrationZone(x, y))
    {
        Serial.print(
            "[TOUCH SELECT] X="
        );

        Serial.print(x);

        Serial.print(
            " Y="
        );

        Serial.println(y);

        Serial.println(
            "[TOUCH] CHOIX = CONFIG PRECEDENTE"
        );

        loadPreviousCalibration();

        calibrationChoiceActive = false;

        choice = 2;

        delay(200);

        return true;
    }


    return false;
}


// ============================================================
// ÉTAT
// ============================================================

bool touchCalibrationChoiceActive()
{
    return calibrationChoiceActive;
}