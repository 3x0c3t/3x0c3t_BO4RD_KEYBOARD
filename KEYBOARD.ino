#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"
#include "keyboard.h"


TFT_eSPI tft = TFT_eSPI();


// ============================================================
// Etat général
// ============================================================

static bool keyboardStarted = false;


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
    // Touch
    // --------------------------------------------------------

    Serial.println("[TOUCH] Init TFT_eSPI");

    Serial.print("[TOUCH] Rotation=");
    Serial.println(SCREEN_ROTATION);


#if TOUCH_CALIBRATION

    Serial.println("[TOUCH] Calibration automatique ACTIVE");

#else

    Serial.println("[TOUCH] Calibration automatique DESACTIVEE");

#endif


    // --------------------------------------------------------
    // Calibration
    // --------------------------------------------------------

    touchCalibrationInit();


#if !TOUCH_CALIBRATION

    keyboardInit();

    keyboardDraw();

    keyboardStarted = true;

#endif
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x;
    uint16_t y;


    // --------------------------------------------------------
    // Lecture tactile
    // --------------------------------------------------------

    if (tft.getTouch(&x, &y))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);

        Serial.print(" Y=");
        Serial.println(y);


        // ----------------------------------------------------
        // Choix calibration
        // ----------------------------------------------------

        if (!keyboardStarted)
        {
            bool finished =
                touchCalibrationUpdate(
                    x,
                    y
                );


            if (finished)
            {
                keyboardInit();

                keyboardDraw();

                keyboardStarted = true;

                Serial.println("[KEYBOARD] Init");
            }


            delay(250);

            return;
        }


        // ----------------------------------------------------
        // Clavier
        // ----------------------------------------------------

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