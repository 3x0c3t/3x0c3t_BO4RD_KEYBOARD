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
    Serial.begin(
        115200
    );

    delay(
        300
    );

    Serial.println();
    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        "  3x0c3t BO4RD KEYBOARD"
    );

    Serial.println(
        "  VERSION 1.1"
    );

    Serial.println(
        "================================"
    );

    // ========================================================
    // TFT
    // ========================================================

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

    // ========================================================
    // DIAGNOSTIC TFT
    // ========================================================

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
        "[TFT] Taille attendue = "
    );

    Serial.print(
        SCREEN_WIDTH
    );

    Serial.print(
        " x "
    );

    Serial.println(
        SCREEN_HEIGHT
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

    if (
        tft.width() == SCREEN_WIDTH &&
        tft.height() == SCREEN_HEIGHT
    )
    {
        Serial.println(
            "[TFT] RESOLUTION OK"
        );
    }
    else
    {
        Serial.println(
            "[TFT] ERREUR RESOLUTION"
        );
    }

    // ========================================================
    // TOUCH
    // ========================================================

    Serial.println();

    Serial.println(
        "[TOUCH] Initialisation..."
    );

    touchCalibrationInit();

    Serial.println(
        "[TOUCH] OK"
    );

    // ========================================================
    // CALIBRATION / EEPROM
    // ========================================================

    Serial.println();

    if (
        FORCE_TOUCH_CALIBRATION
    )
    {
        // ====================================================
        // MODE FORCE
        // ====================================================

        Serial.println(
            "[TOUCH] Calibration forcee"
        );

        Serial.println(
            "[TOUCH] Calibration EEPROM ignoree"
        );

        Serial.println(
            "[TOUCH] Lancement calibration"
        );

        startTouchCalibration();
    }
    else
    {
        // ====================================================
        // MODE NORMAL
        // ====================================================

        Serial.println(
            "[TOUCH] Recherche calibration EEPROM..."
        );

        bool calibrationLoaded =
            loadTouchCalibration();

        if (
            !calibrationLoaded
        )
        {
            Serial.println();

            Serial.println(
                "[TOUCH] Aucune calibration valide"
            );

            Serial.println(
                "[TOUCH] Lancement calibration"
            );

            startTouchCalibration();
        }
        else
        {
            Serial.println();

            Serial.println(
                "[TOUCH] Utilisation calibration EEPROM"
            );
        }
    }

    // ========================================================
    // CLAVIER
    // ========================================================

    Serial.println();

    Serial.println(
        "[KEYBOARD] Initialisation"
    );

    keyboardInit();

    keyboardDraw();

    keyboardStarted =
        true;

    Serial.println(
        "[KEYBOARD] OK"
    );

    Serial.println();

    Serial.println(
        "[SYSTEM] Pret"
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
    // SECURITE
    // ========================================================

    if (
        !keyboardStarted
    )
    {
        return;
    }

    // ========================================================
    // TOUCH CALIBRE
    // ========================================================

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

        Serial.print(
            x
        );

        Serial.print(
            " Y="
        );

        Serial.println(
            y
        );

        // ====================================================
        // SECURITE COORDONNEES
        // ====================================================

        if (
            x < SCREEN_WIDTH &&
            y < SCREEN_HEIGHT
        )
        {
            Serial.print(
                "[KEYBOARD TOUCH] X="
            );

            Serial.print(
                x
            );

            Serial.print(
                " Y="
            );

            Serial.println(
                y
            );

            keyboardUpdate(
                (int16_t)x,
                (int16_t)y
            );
        }

        delay(
            120
        );
    }
}