#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"
#include "keyboard.h"

TFT_eSPI tft = TFT_eSPI();

bool keyboardStarted = false;

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);
    delay(300);

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

    touchCalibrationInit();

    // --------------------------------------------------------
    // CHOIX CALIBRATION
    // --------------------------------------------------------

    Serial.println("[TOUCH] Ecran de choix calibration");

    bool newCalibration = touchCalibrationChoice();

    if (newCalibration)
    {
        Serial.println("[TOUCH] Lancement calibration");

        startTouchCalibration();

        keyboardStarted = false;
    }
    else
    {
        Serial.println("[TOUCH] Configuration precedente chargee");

        keyboardInit();
        keyboardDraw();

        keyboardStarted = true;
    }
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x = 0;
    uint16_t y = 0;

    // --------------------------------------------------------
    // CALIBRATION EN COURS
    // --------------------------------------------------------

    if (touchCalibrationActive())
    {
        if (tft.getTouch(
                &x,
                &y,
                TOUCH_THRESHOLD))
        {
            Serial.print("[TOUCH CAL] X=");
            Serial.print(x);
            Serial.print(" Y=");
            Serial.println(y);

            if (touchCalibrationUpdate(x, y))
            {
                Serial.println("[TOUCH] Calibration terminee");

                keyboardInit();
                keyboardDraw();

                keyboardStarted = true;
            }

            delay(120);
        }

        return;
    }

    // --------------------------------------------------------
    // CLAVIER
    // --------------------------------------------------------

    if (!keyboardStarted)
    {
        return;
    }

    if (tft.getTouch(
            &x,
            &y,
            TOUCH_THRESHOLD))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);
        Serial.print(" Y=");
        Serial.println(y);

        Serial.print("[KEYBOARD TOUCH] X=");
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