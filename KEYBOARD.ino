#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"
#include "keyboard.h"

// ============================================================
// 3x0c3t BO4RD KEYBOARD
// VERSION 1.3
// ============================================================

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
    Serial.println("================================");
    Serial.println("  3x0c3t BO4RD KEYBOARD");
    Serial.println("  VERSION 1.3");
    Serial.println("================================");
    Serial.println();

    // ========================================================
    // TFT
    // ========================================================

    Serial.println("[TFT] Initialisation...");

    tft.init();

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    Serial.println("[TFT] OK");

    Serial.println();
    Serial.println("[TFT] Configuration");

    Serial.print("[TFT] Rotation = ");
    Serial.println(SCREEN_ROTATION);

    Serial.print("[TFT] Taille attendue = ");
    Serial.print(SCREEN_WIDTH);
    Serial.print(" x ");
    Serial.println(SCREEN_HEIGHT);

    Serial.print("[TFT] Taille reelle = ");
    Serial.print(tft.width());
    Serial.print(" x ");
    Serial.println(tft.height());

    if (
        tft.width() == SCREEN_WIDTH &&
        tft.height() == SCREEN_HEIGHT
    )
    {
        Serial.println("[TFT] RESOLUTION OK");
    }
    else
    {
        Serial.println("[TFT] ATTENTION RESOLUTION");
    }

    // ========================================================
    // TOUCH
    // ========================================================

    Serial.println();
    Serial.println("[TOUCH] Initialisation...");
    Serial.println("[DEBUG] Avant touchCalibrationInit()");

    touchCalibrationInit();

    Serial.println("[DEBUG] Apres touchCalibrationInit()");

    Serial.print("[TOUCH] FORCE_TOUCH_CALIBRATION = ");
    Serial.println(FORCE_TOUCH_CALIBRATION);

    // ========================================================
    // CALIBRATION FORCEE
    // ========================================================

#if FORCE_TOUCH_CALIBRATION

    Serial.println("[TOUCH] Calibration forcee");

    keyboardStarted = false;

    Serial.println("[DEBUG] Appel startTouchCalibration()");

    startTouchCalibration();

    Serial.println("[DEBUG] startTouchCalibration() termine");

    return;

#endif

    // ========================================================
    // CHARGEMENT EEPROM
    // ========================================================

    Serial.println(
        "[TOUCH] Recherche calibration EEPROM..."
    );

    if (
        loadPreviousCalibration()
    )
    {
        Serial.println(
            "[TOUCH] Calibration existante chargee"
        );

        Serial.print(
            "[TOUCH] Rotation = "
        );

        Serial.println(
            selectedTouchRotation()
        );

        keyboardInit();

        keyboardDraw();

        keyboardStarted = true;

        Serial.println(
            "[KEYBOARD] Demarrage"
        );

        return;
    }

    // ========================================================
    // AUCUNE CALIBRATION
    // ========================================================

    Serial.println(
        "[TOUCH] Aucune calibration valide"
    );

    Serial.println(
        "[TOUCH] Lancement calibration"
    );

    keyboardStarted = false;

    startTouchCalibration();
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    // ========================================================
    // CALIBRATION EN COURS
    // ========================================================

    if (
        touchCalibrationActive()
    )
    {
        updateTouchCalibration();

        delay(5);
        yield();

        return;
    }

    // ========================================================
    // CALIBRATION TERMINEE
    // ========================================================
    //
    // IMPORTANT :
    // Ce test doit être EN DEHORS de
    // touchCalibrationActive().
    //
    // Une fois DONE, active() retourne false.
    // ========================================================

    if (
        touchCalibrationFinished()
    )
    {
        if (
            !keyboardStarted
        )
        {
            Serial.println();
            Serial.println(
                "[TOUCH] Calibration terminee"
            );

            Serial.print(
                "[TOUCH] Rotation = "
            );

            Serial.println(
                selectedTouchRotation()
            );

            // ------------------------------------------------
            // S'assurer que la rotation finale est active
            // ------------------------------------------------

            tft.setRotation(
                selectedTouchRotation()
            );

            // ------------------------------------------------
            // Initialisation clavier
            // ------------------------------------------------

            keyboardInit();

            keyboardDraw();

            keyboardStarted = true;

            Serial.println(
                "[KEYBOARD] Demarrage"
            );

            Serial.println(
                "[KEYBOARD] Ecran affiche"
            );
        }
    }

    // ========================================================
    // SECURITE
    // ========================================================

    if (
        !keyboardStarted
    )
    {
        delay(5);
        yield();

        return;
    }

    // ========================================================
    // LECTURE TACTILE CLAVIER
    // ========================================================

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

        keyboardUpdate(
            (int16_t)x,
            (int16_t)y
        );

        // ----------------------------------------------------
        // Anti double-clic
        // ----------------------------------------------------

        delay(
            KEYBOARD_TOUCH_DELAY
        );
    }

    yield();
}