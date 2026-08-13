#include "touch_calibration.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"

// ============================================================
// TFT
// ============================================================
//
// L'instance TFT est créée dans KEYBOARD.ino
//
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// CALIBRATION TFT_eSPI
// ============================================================
//
// Ces valeurs sont celles utilisées pour le test.
//
// Si une calibration officielle TFT_eSPI fournit d'autres
// valeurs, remplacer uniquement ce tableau.
//
// ============================================================

static uint16_t calData[5] =
{
    300,
    3500,
    300,
    3500,
    7
};

// ============================================================
// Etat
// ============================================================

static bool touchInitialized = false;

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Init TFT_eSPI");

    // --------------------------------------------------------
    // Rotation
    // --------------------------------------------------------

    tft.setRotation(SCREEN_ROTATION);

    // --------------------------------------------------------
    // Calibration
    // --------------------------------------------------------

    tft.setTouch(calData);

    touchInitialized = true;

    Serial.println("[TOUCH] TFT_eSPI OK");

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);

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
    // TFT_eSPI
    // --------------------------------------------------------

    uint16_t tx;
    uint16_t ty;

    if (!tft.getTouch(&tx, &ty))
        return false;

    // --------------------------------------------------------
    // Vérification coordonnées
    // --------------------------------------------------------

    if (
        tx >= SCREEN_WIDTH ||
        ty >= SCREEN_HEIGHT
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // Conversion
    // --------------------------------------------------------

    *x = (int16_t)tx;
    *y = (int16_t)ty;

    return true;
}