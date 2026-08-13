#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT global
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// Etat
// ============================================================

static bool touchInitialized = false;

// ============================================================
// Initialisation tactile
// ============================================================

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Init TFT_eSPI");

    touchInitialized = false;

    // --------------------------------------------------------
    // TFT_eSPI utilise directement son driver tactile
    // --------------------------------------------------------

    Serial.println("[TOUCH] TFT_eSPI OK");

    tft.setRotation(SCREEN_ROTATION);

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);

    // --------------------------------------------------------
    // Calibration automatique
    // --------------------------------------------------------

#if TOUCH_CALIBRATION

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    delay(500);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        40,
        2
    );

    tft.drawString(
        "Suivre les croix",
        SCREEN_WIDTH / 2,
        65,
        2
    );

    tft.drawString(
        "Toucher le centre",
        SCREEN_WIDTH / 2,
        90,
        2
    );

    delay(500);

    uint16_t calData[5];

    tft.calibrateTouch(
        calData,
        TFT_MAGENTA,
        TFT_BLACK,
        15
    );

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
    // IMPORTANT
    //
    // On conserve la calibration dans TFT_eSPI.
    // --------------------------------------------------------

    tft.setTouch(calData);

    delay(500);

#endif

    touchInitialized = true;

    Serial.println("[TOUCH] Touch READY");
}

// ============================================================
// Lecture tactile
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

    uint16_t tx;
    uint16_t ty;

    // --------------------------------------------------------
    // Lecture TFT_eSPI
    //
    // IMPORTANT :
    // getTouch() retourne directement les coordonnées
    // écran après calibration.
    // --------------------------------------------------------

    if (!tft.getTouch(&tx, &ty))
        return false;

    // --------------------------------------------------------
    // Protection
    // --------------------------------------------------------

    if (tx >= SCREEN_WIDTH)
        tx = SCREEN_WIDTH - 1;

    if (ty >= SCREEN_HEIGHT)
        ty = SCREEN_HEIGHT - 1;

    *x = (int16_t)tx;
    *y = (int16_t)ty;

    Serial.print("[TOUCH] X=");
    Serial.print(*x);

    Serial.print(" Y=");
    Serial.println(*y);

    return true;
}