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
// ETAT SYSTEME
// ============================================================

enum SystemState
{
    SYSTEM_CALIBRATION,
    SYSTEM_KEYBOARD
};

static SystemState systemState =
    SYSTEM_CALIBRATION;

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

#if FORCE_TOUCH_CALIBRATION

    Serial.println(
        "[TOUCH] Calibration forcee"
    );

    Serial.println(
        "[TOUCH] Calibration EEPROM ignoree"
    );

    Serial.println(
        "[TOUCH] Lancement calibration"
    );

    systemState =
        SYSTEM_CALIBRATION;

    startTouchCalibration();

#else

    Serial.println(
        "[TOUCH] Recherche calibration EEPROM..."
    );

    if (
        loadTouchCalibration()
    )
    {
        Serial.println(
            "[TOUCH] Calibration EEPROM chargee"
        );

        systemState =
            SYSTEM_KEYBOARD;
    }
    else
    {
        Serial.println(
            "[TOUCH] Aucune calibration valide"
        );

        Serial.println(
            "[TOUCH] Lancement calibration"
        );

        systemState =
            SYSTEM_CALIBRATION;

        startTouchCalibration();
    }

#endif

    // ========================================================
    // IMPORTANT
    //
    // Le clavier NE DOIT PAS etre initialise ici si une
    // calibration est en cours.
    // ========================================================

    if (
        systemState ==
        SYSTEM_KEYBOARD
    )
    {
        Serial.println();

        Serial.println(
            "[KEYBOARD] Initialisation"
        );

        keyboardInit();

        keyboardDraw();

        Serial.println(
            "[KEYBOARD] OK"
        );

        Serial.println();

        Serial.println(
            "[SYSTEM] Pret"
        );
    }
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    // ========================================================
    // CALIBRATION
    // ========================================================

    if (
        systemState ==
        SYSTEM_CALIBRATION
    )
    {
        uint16_t rawX = 0;
        uint16_t rawY = 0;

        // ----------------------------------------------------
        // Lecture tactile RAW
        // ----------------------------------------------------

        if (
            tft.getTouchRaw(
                &rawX,
                &rawY
            )
        )
        {
            Serial.print(
                "[TOUCH RAW] X="
            );

            Serial.print(
                rawX
            );

            Serial.print(
                " Y="
            );

            Serial.println(
                rawY
            );

            // ------------------------------------------------
            // Petite stabilisation
            // ------------------------------------------------

            delay(
                40
            );

            uint16_t rawX2 = 0;
            uint16_t rawY2 = 0;

            if (
                !tft.getTouchRaw(
                    &rawX2,
                    &rawY2
                )
            )
            {
                yield();

                return;
            }

            // ------------------------------------------------
            // Vérification stabilité
            // ------------------------------------------------

            if (
                abs(
                    (int32_t)rawX2 -
                    (int32_t)rawX
                ) > 100
                ||
                abs(
                    (int32_t)rawY2 -
                    (int32_t)rawY
                ) > 100
            )
            {
                Serial.println(
                    "[TOUCH] Lecture instable ignoree"
                );

                yield();

                return;
            }

            // ------------------------------------------------
            // Validation
            // ------------------------------------------------

            Serial.println(
                "[TOUCH] Pression valide"
            );

            touchCalibrationUpdate(
                rawX2,
                rawY2
            );

            // ------------------------------------------------
            // ATTENDRE LE RELACHEMENT
            // ------------------------------------------------

            delay(
                150
            );

            uint16_t releaseX = 0;
            uint16_t releaseY = 0;

            while (
                tft.getTouchRaw(
                    &releaseX,
                    &releaseY
                )
            )
            {
                delay(
                    20
                );

                yield();
            }

            Serial.println(
                "[TOUCH] Relachement"
            );

            delay(
                250
            );

            // ------------------------------------------------
            // Vérifier si la calibration est terminée
            // ------------------------------------------------

            if (
                touchCalibrationFinished()
            )
            {
                Serial.println();
                Serial.println(
                    "[TOUCH] Calibration terminee"
                );

                Serial.println(
                    "[TOUCH] Passage au clavier"
                );

                systemState =
                    SYSTEM_KEYBOARD;

                delay(
                    500
                );

                // ------------------------------------------------
                // Clavier
                // ------------------------------------------------

                Serial.println();

                Serial.println(
                    "[KEYBOARD] Initialisation"
                );

                keyboardInit();

                keyboardDraw();

                Serial.println(
                    "[KEYBOARD] OK"
                );

                Serial.println();

                Serial.println(
                    "[SYSTEM] Pret"
                );
            }
        }

        yield();

        return;
    }

    // ========================================================
    // CLAVIER
    // ========================================================

    if (
        systemState ==
        SYSTEM_KEYBOARD
    )
    {
        uint16_t x = 0;
        uint16_t y = 0;

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

            // ------------------------------------------------
            // Sécurité coordonnées
            // ------------------------------------------------

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

            // ------------------------------------------------
            // Anti-rebond
            // ------------------------------------------------

            delay(
                120
            );
        }

        yield();

        return;
    }

    yield();
}