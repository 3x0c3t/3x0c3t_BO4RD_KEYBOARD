#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// ============================================================
// BROCHES
// ============================================================

#ifndef TOUCH_CS
#define TOUCH_CS 0
#endif

#ifndef TOUCH_IRQ
#define TOUCH_IRQ 5
#endif

#ifndef TFT_CS
#define TFT_CS 15
#endif

// ============================================================
// XPT2046
// ============================================================

static XPT2046_Touchscreen touch(
    TOUCH_CS,
    TOUCH_IRQ
);

static bool touchInitialized = false;

// ============================================================
// CALIBRATION
// ============================================================

#define TOUCH_MIN_X 200
#define TOUCH_MAX_X 3900

#define TOUCH_MIN_Y 200
#define TOUCH_MAX_Y 3900

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Init");

    // --------------------------------------------------------
    // TFT CS
    // --------------------------------------------------------

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    // --------------------------------------------------------
    // Touch CS
    // --------------------------------------------------------

    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    // --------------------------------------------------------
    // IRQ
    // --------------------------------------------------------

    pinMode(TOUCH_IRQ, INPUT_PULLUP);

    // --------------------------------------------------------
    // SPI
    // --------------------------------------------------------

    SPI.begin();

    delay(50);

    // --------------------------------------------------------
    // Initialisation XPT2046
    // --------------------------------------------------------

    if (!touch.begin())
    {
        touchInitialized = false;

        Serial.println("[TOUCH] ERREUR");

        return;
    }

    touchInitialized = true;

    touch.setRotation(2);

    Serial.println("[TOUCH] OK");

    Serial.print("[TOUCH] CS=");
    Serial.println(TOUCH_CS);

    Serial.print("[TOUCH] IRQ=");
    Serial.println(TOUCH_IRQ);
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
    // LA BIBLIOTHEQUE GERE LA DETECTION
    // --------------------------------------------------------

    if (!touch.touched())
        return false;

    // --------------------------------------------------------
    // LECTURE
    // --------------------------------------------------------

    TS_Point p = touch.getPoint();

    int32_t rawX = p.x;
    int32_t rawY = p.y;
    int32_t rawZ = p.z;

    // --------------------------------------------------------
    // DEBUG
    // --------------------------------------------------------

    Serial.print("[TOUCH RAW] X=");
    Serial.print(rawX);

    Serial.print(" Y=");
    Serial.print(rawY);

    Serial.print(" Z=");
    Serial.println(rawZ);

    // --------------------------------------------------------
    // VALIDATION
    // --------------------------------------------------------

    if (
        rawX <= 0 ||
        rawX >= 4095 ||
        rawY <= 0 ||
        rawY >= 4095
    )
    {
        Serial.println("[TOUCH] RAW invalide");

        return false;
    }

    // --------------------------------------------------------
    // PRESSION
    // --------------------------------------------------------

    if (rawZ <= 0)
    {
        Serial.println("[TOUCH] Z invalide");

        return false;
    }

    // --------------------------------------------------------
    // CONVERSION
    // --------------------------------------------------------

    int32_t screenX = map(
        rawX,
        TOUCH_MIN_X,
        TOUCH_MAX_X,
        239,
        0
    );

    int32_t screenY = map(
        rawY,
        TOUCH_MIN_Y,
        TOUCH_MAX_Y,
        319,
        0
    );

    // --------------------------------------------------------
    // LIMITES
    // --------------------------------------------------------

    screenX = constrain(
        screenX,
        0,
        239
    );

    screenY = constrain(
        screenY,
        0,
        319
    );

    *x = (int16_t)screenX;
    *y = (int16_t)screenY;

    // --------------------------------------------------------
    // RESULTAT
    // --------------------------------------------------------

    Serial.print("[TOUCH] X=");
    Serial.print(*x);

    Serial.print(" Y=");
    Serial.println(*y);

    return true;
}