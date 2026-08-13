#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT GLOBAL
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static bool touchInitialized = false;

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Init TFT_eSPI");

    touchInitialized = false;

    // --------------------------------------------------------
    // Rotation
    // --------------------------------------------------------

    tft.setRotation(SCREEN_ROTATION);

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);

    // --------------------------------------------------------
    // Calibration
    // --------------------------------------------------------

#if TOUCH_CALIBRATION

    Serial.println("[TOUCH] Calibration automatique ACTIVE");

    delay(500);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
    );

    tft.setTextDatum(MC_DATUM);

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
        "precisement au centre",
        SCREEN_WIDTH / 2,
        72,
        2
    );

    tft.setTextDatum(TL_DATUM);

    delay(1000);

    // --------------------------------------------------------
    // Calibration TFT_eSPI
    // --------------------------------------------------------

    uint16_t calData[5];

    tft.calibrateTouch(
        calData,
        TFT_MAGENTA,
        TFT_BLACK,
        15
    );

    // --------------------------------------------------------
    // Affichage des valeurs
    // --------------------------------------------------------

    Serial.println();
    Serial.println("==============================");
    Serial.println("CALIBRATION TERMINEE");
    Serial.println("==============================");

    Serial.print("calData[0] = ");
    Serial.println(calData[0]);

    Serial.print("calData[1] = ");
    Serial.println(calData[1]);

    Serial.print("calData[2] = ");
    Serial.println(calData[2]);

    Serial.print("calData[3] = ");
    Serial.println(calData[3]);

    Serial.print("calData[4] = ");
    Serial.println(calData[4]);

    Serial.println("==============================");

    // --------------------------------------------------------
    // Appliquer la calibration
    // --------------------------------------------------------

    tft.setTouch(calData);

    // --------------------------------------------------------
    // Rotation finale
    // --------------------------------------------------------

    tft.setRotation(SCREEN_ROTATION);

    delay(300);

#else

    Serial.println("[TOUCH] Calibration automatique DESACTIVEE");

#endif

    // --------------------------------------------------------
    // Pret
    // --------------------------------------------------------

    touchInitialized = true;

    Serial.println("[TOUCH] Touch READY");
}

// ============================================================
// LECTURE TACTILE
// ============================================================

bool touchReadScreen(
    int16_t* x,
    int16_t* y
)
{
    if (x == nullptr || y == nullptr)
        return false;

    *x = -1;
    *y = -1;

    if (!touchInitialized)
        return false;

    // --------------------------------------------------------
    // Lecture TFT_eSPI
    // --------------------------------------------------------

    uint16_t tx;
    uint16_t ty;

    if (!tft.getTouch(&tx, &ty))
        return false;

    // --------------------------------------------------------
    // Limites
    // --------------------------------------------------------

    if (tx >= SCREEN_WIDTH)
        tx = SCREEN_WIDTH - 1;

    if (ty >= SCREEN_HEIGHT)
        ty = SCREEN_HEIGHT - 1;

    // --------------------------------------------------------
    // Coordonnees ecran
    // --------------------------------------------------------

    *x = (int16_t)tx;
    *y = (int16_t)ty;

    // --------------------------------------------------------
    // Debug
    // --------------------------------------------------------

    Serial.print("[TOUCH] X=");
    Serial.print(*x);

    Serial.print(" Y=");
    Serial.println(*y);

    return true;
}