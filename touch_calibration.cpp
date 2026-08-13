#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"


extern TFT_eSPI tft;


// ============================================================
// Donnees calibration
// ============================================================

static uint16_t calData[5] = {
    TOUCH_PREVIOUS_CAL_0,
    TOUCH_PREVIOUS_CAL_1,
    TOUCH_PREVIOUS_CAL_2,
    TOUCH_PREVIOUS_CAL_3,
    TOUCH_PREVIOUS_CAL_4
};


// ============================================================
// Etat
// ============================================================

static bool calibrationChoiceActive = false;
static bool calibrationFinished = false;


// ============================================================
// Chargement calibration precedente
// ============================================================

void loadPreviousCalibration()
{
    calData[0] = TOUCH_PREVIOUS_CAL_0;
    calData[1] = TOUCH_PREVIOUS_CAL_1;
    calData[2] = TOUCH_PREVIOUS_CAL_2;
    calData[3] = TOUCH_PREVIOUS_CAL_3;
    calData[4] = TOUCH_PREVIOUS_CAL_4;

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
// Dessin d'une croix
// ============================================================

static void drawCross(
    int16_t x,
    int16_t y
)
{
    const int size = 8;

    tft.drawLine(
        x - size,
        y,
        x + size,
        y,
        COLOR_RED
    );

    tft.drawLine(
        x,
        y - size,
        x,
        y + size,
        COLOR_RED
    );

    tft.drawCircle(
        x,
        y,
        3,
        COLOR_RED
    );
}


// ============================================================
// Ecran de choix
// ============================================================

static void drawCalibrationChoiceScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);


    // --------------------------------------------------------
    // Titre
    // --------------------------------------------------------

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

    tft.drawString(
        "Choisissez une option",
        SCREEN_WIDTH / 2,
        50,
        2
    );


    // --------------------------------------------------------
    // Bouton nouvelle calibration
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

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLUE
    );

    tft.drawString(
        "NOUVELLE",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 22,
        2
    );

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 42,
        2
    );


    // --------------------------------------------------------
    // Bouton configuration précédente
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

    tft.setTextColor(
        COLOR_BLACK,
        COLOR_GREEN
    );

    tft.drawString(
        "CONFIGURATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 22,
        2
    );

    tft.drawString(
        "PRECEDENTE",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 42,
        2
    );


    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );
}


// ============================================================
// Initialisation
// ============================================================

void touchCalibrationInit()
{
    calibrationFinished = false;
    calibrationChoiceActive = false;


#if TOUCH_CALIBRATION

    Serial.println("[TOUCH] Ecran de choix calibration");

    drawCalibrationChoiceScreen();

    calibrationChoiceActive = true;


#else

    Serial.println("[TOUCH] Calibration DESACTIVEE");

    loadPreviousCalibration();

    tft.setTouch(calData);

    calibrationFinished = true;

#endif
}


// ============================================================
// Test zone nouvelle calibration
// ============================================================

static bool isNewCalibrationZone(
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
// Test zone configuration précédente
// ============================================================

static bool isPreviousCalibrationZone(
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
// Nouvelle calibration
// ============================================================

static void startNewCalibration()
{
    calibrationChoiceActive = false;

    Serial.println("[TOUCH] CHOIX = NOUVELLE CALIBRATION");

    delay(250);

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

    tft.drawString(
        "Touchez les croix",
        SCREEN_WIDTH / 2,
        50,
        2
    );

    tft.drawString(
        "entre leurs centres",
        SCREEN_WIDTH / 2,
        72,
        2
    );

    tft.setTextColor(
        COLOR_RED,
        COLOR_BLACK
    );

    tft.drawString(
        "4 angles",
        SCREEN_WIDTH / 2,
        100,
        2
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    delay(500);

    Serial.println("[TOUCH] Lancement calibration");

    tft.calibrateTouch(
        calData,
        COLOR_RED,
        COLOR_BLACK,
        15
    );

    tft.setTouch(calData);

    Serial.println("=== CALIBRATION TERMINE ===");

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

    calibrationFinished = true;

    delay(500);
}


// ============================================================
// Utiliser calibration précédente
// ============================================================

static void usePreviousCalibration()
{
    calibrationChoiceActive = false;

    Serial.println("[TOUCH] CHOIX = CONFIGURATION PRECEDENTE");

    loadPreviousCalibration();

    tft.setTouch(calData);

    Serial.println("[TOUCH] Configuration precedente appliquee");

    calibrationFinished = true;

    delay(300);

    Serial.println("[TOUCH] Passage au clavier");
}


// ============================================================
// Traitement tactile
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    if (!calibrationChoiceActive)
    {
        return calibrationFinished;
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
        startNewCalibration();

        return true;
    }


    // --------------------------------------------------------
    // CONFIGURATION PRECEDENTE
    // --------------------------------------------------------

    if (isPreviousCalibrationZone(x, y))
    {
        usePreviousCalibration();

        return true;
    }


    Serial.println("[TOUCH] Clic hors zone");

    return false;
}


// ============================================================
// Etat calibration
// ============================================================

bool touchCalibrationIsFinished()
{
    return calibrationFinished;
}


// ============================================================
// Donnees calibration
// ============================================================

uint16_t *touchCalibrationData()
{
    return calData;
}