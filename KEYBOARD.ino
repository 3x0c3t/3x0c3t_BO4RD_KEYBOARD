#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"
#include "keyboard.h"

// ============================================================
// TFT
// ============================================================

TFT_eSPI tft = TFT_eSPI();

// ============================================================
// ETAT
// ============================================================

static bool keyboardStarted = false;

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);
    delay(300);

    Serial.println();
    Serial.println(
        "=== 3x0c3t KEYBOARD v1.0 ==="
    );

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

    touchCalibrationInit();

    // --------------------------------------------------------
    // On attend le choix utilisateur
    // --------------------------------------------------------

    keyboardStarted = false;
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x = 0;
    uint16_t y = 0;

    // ========================================================
    // PHASE CHOIX CALIBRATION
    // ========================================================

    if (!keyboardStarted)
    {
        if (touchCalibrationIsRunning())
        {
            delay(10);
            return;
        }

        if (touchCalibrationUseKeyboard())
        {
            Serial.println(
                "[KEYBOARD] Passage au clavier"
            );

            keyboardInit();

            Serial.println(
                "[KEYBOARD] Init termine"
            );

            keyboardDraw();

            keyboardStarted = true;

            delay(300);

            return;
        }

        // ----------------------------------------------------
        // Lecture touch
        // ----------------------------------------------------

        if (tft.getTouch(
                &x,
                &y,
                TOUCH_THRESHOLD))
        {
            Serial.print("[TOUCH SELECT] X=");
            Serial.print(x);

            Serial.print(" Y=");
            Serial.println(y);

            touchCalibrationUpdate(
                x,
                y
            );

            delay(250);
        }

        return;
    }

    // ========================================================
    // PHASE CLAVIER
    // ========================================================

    if (tft.getTouch(
            &x,
            &y,
            TOUCH_THRESHOLD))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);

        Serial.print(" Y=");
        Serial.println(y);

        Serial.print(
            "[KEYBOARD TOUCH] X="
        );
        Serial.print(x);

        Serial.print(" Y=");
        Serial.println(y);

        keyboardUpdate(
            (int16_t)x,
            (int16_t)y
        );

        delay(120);
    }
}