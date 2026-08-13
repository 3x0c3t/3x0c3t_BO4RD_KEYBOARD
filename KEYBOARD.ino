#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

// ============================================================
// TFT
// ============================================================

TFT_eSPI tft = TFT_eSPI();

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("=== 3x0c3t KEYBOARD v1.0 ===");

    // --------------------------------------------------------
    // TFT
    // --------------------------------------------------------

    Serial.println("[KEYBOARD] TFT Init");

    tft.init();

    tft.setRotation(SCREEN_ROTATION);

    tft.fillScreen(COLOR_BLACK);

    Serial.println("[KEYBOARD] TFT OK");

    // --------------------------------------------------------
    // TOUCH
    // --------------------------------------------------------

    Serial.println("[TOUCH] Init TFT_eSPI");

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);

    // --------------------------------------------------------
    // CALIBRATION
    // --------------------------------------------------------

#if TOUCH_CALIBRATION == 1

    Serial.println("[TOUCH] Calibration automatique ACTIVE");

    touchCalibration();

#else

    Serial.println("[TOUCH] Calibration desactivee");

    touchLoadPreviousCalibration();

#endif

    // --------------------------------------------------------
    // KEYBOARD
    // --------------------------------------------------------

    Serial.println("[KEYBOARD] Initialisation");

    keyboardInit();

    Serial.println("[KEYBOARD] OK");

    Serial.println("=== KEYBOARD READY ===");
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t touchX = 0;
    uint16_t touchY = 0;

    if (tft.getTouch(&touchX, &touchY))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(touchX);

        Serial.print(" Y=");
        Serial.println(touchY);

        keyboardUpdate(
            (int16_t)touchX,
            (int16_t)touchY
        );

        delay(120);
    }
}