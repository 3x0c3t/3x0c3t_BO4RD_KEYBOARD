#include "touch_calibration.h"
#include "config.h"

#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// ============================================================
// XPT2046
// ============================================================

static XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// ============================================================
// Initialisation
// ============================================================

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Initialisation");

    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    // SPI matériel ESP8266
    // IMPORTANT :
    // SPI.begin() ne prend aucun paramètre sur ESP8266.
    SPI.begin();

    if (touch.begin())
    {
        Serial.println("[TOUCH] XPT2046 OK");
    }
    else
    {
        Serial.println("[TOUCH] XPT2046 ERREUR");
    }

    touch.setRotation(TFT_ROTATION);

    Serial.println("[TOUCH] Rotation 2");
}

// ============================================================
// Lecture RAW
// ============================================================

bool touchReadRaw(
    int16_t &rawX,
    int16_t &rawY,
    int16_t &rawZ
)
{
    if (!touch.touched())
    {
        return false;
    }

    TS_Point point = touch.getPoint();

    rawX = point.x;
    rawY = point.y;

    // La bibliothèque XPT2046 retourne z
    rawZ = point.z;

    return true;
}

// ============================================================
// Conversion RAW -> écran
// ============================================================

bool touchReadScreen(
    int16_t &screenX,
    int16_t &screenY
)
{
    int16_t rawX;
    int16_t rawY;
    int16_t rawZ;

    if (!touchReadRaw(rawX, rawY, rawZ))
    {
        return false;
    }

    if (rawZ < TOUCH_PRESSURE_MIN)
    {
        return false;
    }

    // --------------------------------------------------------
    // Conversion brute vers écran.
    //
    // Pour une XPT2046 montée différemment, il peut être
    // nécessaire d'inverser X/Y.
    //
    // La rotation logicielle XPT2046 est déjà configurée
    // avec setRotation(2).
    // --------------------------------------------------------

    screenX = map(
        rawX,
        TOUCH_MIN_X,
        TOUCH_MAX_X,
        0,
        SCREEN_WIDTH - 1
    );

    screenY = map(
        rawY,
        TOUCH_MIN_Y,
        TOUCH_MAX_Y,
        0,
        SCREEN_HEIGHT - 1
    );

    screenX = constrain(
        screenX,
        0,
        SCREEN_WIDTH - 1
    );

    screenY = constrain(
        screenY,
        0,
        SCREEN_HEIGHT - 1
    );

    return true;
}