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

bool keyboardStarted = false;

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(300);

    Serial.println();
    Serial.println(
        "================================"
    );
    Serial.println(
        "  3x0c3t BO4RD KEYBOARD"
    );
    Serial.println(
        "  VERSION 1.3"
    );
    Serial.println(
        "================================"
    );

    // ========================================================
    // TFT
    // ========================================================

    Serial.println();
    Serial.println(
        "[TFT] Initialisation..."
    );

    tft.init();

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    Serial.println(
        "[TFT] OK"
    );

    Serial.println();
    Serial.println(
        "[TFT] Configuration"
    );

    Serial.print(
        "[TFT] Rotation = "
    );

    Serial.println(
        SCREEN_ROTATION
    );

    Serial.print(
        "[TFT] Taille reelle = "
    );

    Serial.print(
        tft.width()
    );

    Serial.print(
        " x "
    );

    Serial.println(
        tft.height()
    );

    // ========================================================
    // TOUCH
    // ========================================================

    Serial.println();

    Serial.println(
        "[TOUCH] Initialisation..."
    );

    touchCalibrationInit();

    Serial.println(
        "[TOUCH] Calibration Init"
    );

    // ========================================================
    // CHOIX CALIBRATION
    // ========================================================

    bool newCalibration =
        touchCalibrationChoice();

    if (
        newCalibration
    )
    {
        Serial.println(
            "[TOUCH] Lancement calibration"
        );

        startTouchCalibration();

        keyboardStarted =
            false;

        return;
    }

    // ========================================================
    // CALIBRATION EXISTANTE
    // ========================================================

    Serial.println(
        "[TOUCH] Calibration existante chargee"
    );

    keyboardInit();

    keyboardDraw();

    keyboardStarted =
        true;

    Serial.println(
        "[KEYBOARD] Demarrage"
    );
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x = 0;
    uint16_t y = 0;

    // ========================================================
    // CALIBRATION EN COURS
    // ========================================================

    if (
        touchCalibrationActive()
    )
    {
        if (
            tft.getTouch(
                &x,
                &y,
                TOUCH_THRESHOLD
            )
        )
        {
            Serial.print(
                "[TOUCH CAL] X="
            );

            Serial.print(x);

            Serial.print(
                " Y="
            );

            Serial.println(y);

            if (
                touchCalibrationUpdate(
                    x,
                    y
                )
            )
            {
                Serial.println(
                    "[TOUCH] Calibration terminee"
                );

                keyboardInit();

                keyboardDraw();

                keyboardStarted =
                    true;

                Serial.println(
                    "[KEYBOARD] Demarrage"
                );
            }

            delay(120);
        }

        return;
    }

    // ========================================================
    // CLAVIER
    // ========================================================

    if (
        !keyboardStarted
    )
    {
        return;
    }

    if (
        tft.getTouch(
            &x,
            &y,
            TOUCH_THRESHOLD
        )
    )
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
            (int16_t)x,
            (int16_t)y
        );

        delay(120);
    }
}