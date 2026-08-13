#include "touch_calibration.h"
#include "config.h"

#include <TFT_eSPI.h>

extern TFT_eSPI tft;


// ============================================================
// Donnees calibration
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
// Etat
// ============================================================

enum TouchCalibrationState
{
    CAL_STATE_DISABLED,
    CAL_STATE_CHOICE,
    CAL_STATE_RUNNING,
    CAL_STATE_FINISHED,
    CAL_STATE_PREVIOUS
};

static TouchCalibrationState calibrationState =
    CAL_STATE_DISABLED;


// ============================================================
// Logs
// ============================================================

static void logCalibration(const char* message)
{
    Serial.print("[TOUCH] ");
    Serial.println(message);
}


// ============================================================
// Charger calibration precedente
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
// Appliquer calibration precedente
// ============================================================

void applyPreviousCalibration()
{
    tft.setTouch(calData);

    calibrationState = CAL_STATE_PREVIOUS;

    logCalibration("Configuration precedente chargee");
}


// ============================================================
// Ecran de choix
// ============================================================

static void drawCalibrationChoiceScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.drawString(
        "CALIBRATION TACTILE",
        SCREEN_WIDTH / 2,
        25,
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
        "NOUVELLE CALIBRATION",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 22,
        2
    );

    tft.drawString(
        "4 ANGLES",
        SCREEN_WIDTH / 2,
        CALIBRATION_NEW_Y + 47,
        2
    );


    // --------------------------------------------------------
    // Bouton configuration precedente
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
        "CONFIG PRECEDENTE",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 22,
        2
    );

    tft.drawString(
        "PASSER AU CLAVIER",
        SCREEN_WIDTH / 2,
        CALIBRATION_REUSE_Y + 47,
        2
    );


    // --------------------------------------------------------
    // Information
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_YELLOW,
        COLOR_BLACK
    );

    tft.drawString(
        "Calibration modifiable dans config.h",
        SCREEN_WIDTH / 2,
        295,
        1
    );
}


// ============================================================
// Initialisation
// ============================================================

void touchCalibrationInit()
{
    Serial.println("[TOUCH] Init TFT_eSPI");

    tft.setRotation(SCREEN_ROTATION);

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);


#if TOUCH_CALIBRATION

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    loadPreviousCalibration();

    drawCalibrationChoiceScreen();

    calibrationState = CAL_STATE_CHOICE;

    Serial.println(
        "[TOUCH] Attente choix utilisateur"
    );

#else

    Serial.println(
        "[TOUCH] Calibration DESACTIVEE"
    );

    loadPreviousCalibration();

    applyPreviousCalibration();

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
// Test zone configuration precedente
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
// Choix utilisateur
// ============================================================

bool touchCalibrationChoice(
    uint16_t x,
    uint16_t y
)
{
    if (calibrationState != CAL_STATE_CHOICE)
    {
        return true;
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

        applyPreviousCalibration();

        Serial.println(
            "[TOUCH] Passage au clavier"
        );

        return true;
    }


    return false;
}


// ============================================================
// Lancement nouvelle calibration
// ============================================================

void startTouchCalibration()
{
    calibrationState = CAL_STATE_RUNNING;

    Serial.println(
        "[TOUCH] Lancement calibration"
    );

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
        "Calibration en cours",
        SCREEN_WIDTH / 2,
        100,
        2
    );


    // --------------------------------------------------------
    // Calibration TFT_eSPI
    // --------------------------------------------------------

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );


    // --------------------------------------------------------
    // Appliquer
    // --------------------------------------------------------

    tft.setTouch(calData);

    calibrationState = CAL_STATE_FINISHED;


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
}


// ============================================================
// Mise a jour calibration
//
// Cette fonction est presente pour permettre au .ino de
// fonctionner avec une boucle de gestion tactile.
// La calibration TFT_eSPI elle-meme est bloquante pendant
// l'appel a calibrateTouch().
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    if (calibrationState == CAL_STATE_CHOICE)
    {
        return touchCalibrationChoice(x, y);
    }

    return
        calibrationState == CAL_STATE_FINISHED ||
        calibrationState == CAL_STATE_PREVIOUS;
}


// ============================================================
// Etat
// ============================================================

bool touchCalibrationIsWaiting()
{
    return calibrationState == CAL_STATE_CHOICE;
}


bool touchCalibrationIsRunning()
{
    return calibrationState == CAL_STATE_RUNNING;
}


bool touchCalibrationIsFinished()
{
    return
        calibrationState == CAL_STATE_FINISHED ||
        calibrationState == CAL_STATE_PREVIOUS;
}


bool touchCalibrationUsePrevious()
{
    return calibrationState == CAL_STATE_PREVIOUS;
}