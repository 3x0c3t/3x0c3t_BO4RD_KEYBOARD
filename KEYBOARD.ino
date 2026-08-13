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

    delay(300);


    Serial.println();
    Serial.println(
        "=== 3x0c3t KEYBOARD v1.0 ==="
    );


    // --------------------------------------------------------
    // TFT
    // --------------------------------------------------------

    Serial.println(
        "[KEYBOARD] TFT Init"
    );

    tft.init();

    tft.setRotation(
        SCREEN_ROTATION
    );

    Serial.println(
        "[KEYBOARD] TFT OK"
    );


    // --------------------------------------------------------
    // TOUCH / CALIBRATION
    // --------------------------------------------------------

    Serial.println(
        "[TOUCH] Init TFT_eSPI"
    );


    Serial.print(
        "[TOUCH] Rotation="
    );

    Serial.println(
        SCREEN_ROTATION
    );


#if TOUCH_CALIBRATION == 1

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    touchCalibration();

#else

    Serial.println(
        "[TOUCH] Calibration automatique DESACTIVEE"
    );

    touchCalibration();

#endif


    // --------------------------------------------------------
    // KEYBOARD
    // --------------------------------------------------------

    Serial.println(
        "[KEYBOARD] Initialisation"
    );

    keyboardBegin();


    Serial.println(
        "[KEYBOARD] OK"
    );


    Serial.println(
        "=== KEYBOARD READY ==="
    );
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x;
    uint16_t y;


    if (tft.getTouch(&x, &y))
    {
        Serial.print(
            "[TOUCH] X="
        );

        Serial.print(x);

        Serial.print(
            " Y="
        );

        Serial.println(y);


        keyboardUpdate(
            x,
            y
        );


        // ----------------------------------------------------
        // Attendre le relâchement
        // ----------------------------------------------------

        while (tft.getTouch(&x, &y))
        {
            delay(10);
        }


        delay(30);
    }
}