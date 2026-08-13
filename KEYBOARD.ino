#include <Arduino.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

// ============================================================
// 3x0c3t BO4RD - KEYBOARD
// ============================================================

static unsigned long lastTouchTime = 0;

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(300);

    Serial.println();
    Serial.println();
    Serial.println("================================");
    Serial.println("  3x0c3t BO4RD KEYBOARD");
    Serial.println("================================");

    Serial.println("[SYSTEM] Initialisation");

    // --------------------------------------------------------
    // Tactile
    // --------------------------------------------------------

    touchCalibrationBegin();

    // --------------------------------------------------------
    // Clavier
    // --------------------------------------------------------

    keyboardBegin();

    Serial.println("[SYSTEM] Pret");
    Serial.println();
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    int16_t x;
    int16_t y;

    if (touchReadScreen(x, y))
    {
        unsigned long now = millis();

        if (
            now - lastTouchTime >=
            TOUCH_DEBOUNCE_MS
        )
        {
            lastTouchTime = now;

            Serial.print("[TOUCH] X=");
            Serial.print(x);

            Serial.print(" Y=");
            Serial.println(y);

            keyboardUpdate(
                x,
                y
            );
        }

        // Attendre la fin de l'appui
        delay(20);
    }

    // --------------------------------------------------------
    // Validation
    // --------------------------------------------------------

    if (keyboardWasValidated())
    {
        Serial.print("[TEXT] ");

        Serial.println(
            keyboardGetText()
        );

        keyboardClearValidated();
    }

    delay(5);
}