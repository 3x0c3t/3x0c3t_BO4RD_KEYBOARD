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
    Serial.println("==============================");
    Serial.println("  3x0c3t KEYBOARD v1.0");
    Serial.println("==============================");

    // --------------------------------------------------------
    // TFT
    // --------------------------------------------------------

    Serial.println("[KEYBOARD] TFT Init");

    tft.init();

    tft.setRotation(2);

    tft.fillScreen(TFT_BLACK);

    Serial.println("[KEYBOARD] TFT OK");

    // --------------------------------------------------------
    // TOUCH
    // --------------------------------------------------------

    touchCalibrationBegin();

    // --------------------------------------------------------
    // KEYBOARD
    // --------------------------------------------------------

    Serial.println("[KEYBOARD] Initialisation");

    keyboardBegin();

    Serial.println("[KEYBOARD] OK");

    Serial.println("==============================");
    Serial.println("  KEYBOARD READY");
    Serial.println("==============================");
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    int16_t touchX;
    int16_t touchY;

    // --------------------------------------------------------
    // Lecture tactile
    // --------------------------------------------------------

    if (touchReadScreen(&touchX, &touchY))
    {
        Serial.print("[KEYBOARD TOUCH] X=");
        Serial.print(touchX);

        Serial.print(" Y=");
        Serial.println(touchY);

        keyboardUpdate(
            touchX,
            touchY
        );
    }

    // --------------------------------------------------------
    // Validation
    // --------------------------------------------------------

    if (keyboardWasValidated())
    {
        Serial.print("[KEYBOARD] Texte valide : ");

        Serial.println(
            keyboardGetText()
        );

        keyboardClearValidated();
    }

    delay(20);
}