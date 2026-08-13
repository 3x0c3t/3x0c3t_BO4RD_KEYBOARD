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

    // Rotation 2 : 240 x 320
    tft.setRotation(2);

    tft.fillScreen(TFT_BLACK);

    Serial.println("[KEYBOARD] TFT OK");

    // --------------------------------------------------------
    // TOUCH
    // --------------------------------------------------------

    Serial.println("[TOUCH] Init");

    touchInit();

    Serial.println("[TOUCH] OK");

    // --------------------------------------------------------
    // KEYBOARD
    // --------------------------------------------------------

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
    int16_t x = 0;
    int16_t y = 0;

    // --------------------------------------------------------
    // Lecture tactile
    // --------------------------------------------------------

    if (touchReadScreen(&x, &y))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);

        Serial.print(" Y=");
        Serial.println(y);

        keyboardUpdate(x, y);

        // ----------------------------------------------------
        // Validation
        // ----------------------------------------------------

        if (keyboardWasValidated())
        {
            Serial.print("[KEYBOARD] Texte valide : ");
            Serial.println(keyboardGetText());

            keyboardClearValidated();
        }
    }

    delay(10);
}