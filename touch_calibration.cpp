#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// DONNEES CALIBRATION
// ============================================================

static uint16_t calData[5] =
{
    TOUCH_PREVIOUS_CAL_0,
    TOUCH_PREVIOUS_CAL_1,
    TOUCH_PREVIOUS_CAL_2,
    TOUCH_PREVIOUS_CAL_3,
    TOUCH_PREVIOUS_CAL_4
};

// ============================================================
// ETAT
// ============================================================

static TouchCalibrationState calibrationState =
    TOUCH_CALIBRATION_CHOICE;

static bool calibrationRunning = false;
static bool calibrationComplete = false;
static bool useKeyboard = false;

// ============================================================
// ETAT CALIBRATION
// ============================================================

bool touchCalibrationIsRunning()
{
    return calibrationRunning;
}

bool touchCalibrationIsComplete()
{
    return calibrationComplete;
}

bool touchCalibrationUseKeyboard()
{
    return useKeyboard;
}

// ============================================================
// CONFIG PRECEDENTE
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
    Serial.println(calData[0]);

    Serial.print("[TOUCH] CAL1=");
    Serial.println(calData[1]);

    Serial.print("[TOUCH] CAL2=");
    Serial.println(calData[2]);

    Serial.print("[TOUCH] CAL3=");
    Serial.println(calData[3]);

    Serial.print("[TOUCH] CAL4=");
    Serial.println(calData[4]);
}

// ============================================================
// ECRAN CHOIX
// ============================================================

void drawCalibrationChoiceScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.setTextSize(2);

    tft.drawString(
        "CONFIGURATION DU TOUCH",
        SCREEN_WIDTH / 2,
        25
    );

    // --------------------------------------------------------
    // BOUTON NOUVELLE CALIBRATION
    // --------------------------------------------------------

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
    tft.setTextSize(2);

    tft.drawString(
        "NOUVELLE CALIBRATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + CALIBRATION_NEW_H / 2
    );

    // --------------------------------------------------------
    // BOUTON CONFIG PRECEDENTE
    // --------------------------------------------------------

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

    tft.setTextColor(COLOR_BLACK, COLOR_GREEN);
    tft.setTextSize(2);

    tft.drawString(
        "CONFIG PRECEDENTE",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + CALIBRATION_REUSE_H / 2
    );

    // --------------------------------------------------------
    // INFORMATION
    // --------------------------------------------------------

    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.setTextSize(1);

    tft.drawString(
        "Choisissez une configuration",
        SCREEN_WIDTH / 2,
        215
    );

    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// INIT
// ============================================================

void touchCalibrationInit()
{
    calibrationState = TOUCH_CALIBRATION_CHOICE;

    calibrationRunning = false;
    calibrationComplete = false;
    useKeyboard = false;

    tft.setRotation(SCREEN_ROTATION);

    Serial.println("[TOUCH] Init TFT_eSPI");
    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    loadPreviousCalibration();

    drawCalibrationChoiceScreen();

    Serial.println(
        "[TOUCH] Attente choix utilisateur"
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
// ZONE CONFIG PRECEDENTE
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
// CHOIX
// ============================================================

bool touchCalibrationChoice()
{
    drawCalibrationChoiceScreen();

    calibrationState = TOUCH_CALIBRATION_CHOICE;
    calibrationRunning = false;
    calibrationComplete = false;
    useKeyboard = false;

    return true;
}

// ============================================================
// DEMARRAGE NOUVELLE CALIBRATION
// ============================================================

void startTouchCalibration()
{
    Serial.println(
        "[TOUCH] Lancement calibration"
    );

    calibrationRunning = true;
    calibrationComplete = false;
    useKeyboard = false;

    calibrationState =
        TOUCH_CALIBRATION_RUNNING;

    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(2);

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        30
    );

    tft.setTextSize(1);

    tft.drawString(
        "Touchez les 4 cibles",
        SCREEN_WIDTH / 2,
        52
    );

    tft.setTextDatum(TL_DATUM);

    delay(500);

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    tft.setTouch(calData);

    Serial.println(
        "[TOUCH] Calibration terminee"
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

    calibrationRunning = false;
    calibrationComplete = true;

    calibrationState =
        TOUCH_CALIBRATION_KEYBOARD;

    useKeyboard = true;
}

// ============================================================
// REUTILISER CONFIG
// ============================================================

bool touchReusePreviousCalibration()
{
    loadPreviousCalibration();

    calibrationRunning = false;
    calibrationComplete = true;
    useKeyboard = true;

    calibrationState =
        TOUCH_CALIBRATION_KEYBOARD;

    Serial.println(
        "[TOUCH] Passage au clavier"
    );

    return true;
}

// ============================================================
// UPDATE
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    if (calibrationState !=
        TOUCH_CALIBRATION_CHOICE)
    {
        return false;
    }

    Serial.print("[TOUCH SELECT] X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);

    // --------------------------------------------------------
    // NOUVELLE CALIBRATION
    // --------------------------------------------------------

    if (isNewCalibrationZone(x, y))
    {
        Serial.println(
            "[TOUCH] CHOIX = NOUVELLE CALIBRATION"
        );

        startTouchCalibration();

        return true;
    }

    // --------------------------------------------------------
    // CONFIG PRECEDENTE
    // --------------------------------------------------------

    if (isPreviousCalibrationZone(x, y))
    {
        Serial.println(
            "[TOUCH] CHOIX = CONFIG PRECEDENTE"
        );

        touchReusePreviousCalibration();

        return true;
    }

    return false;
}