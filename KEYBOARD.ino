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
    Serial.println();
    Serial.println("================================");
    Serial.println("  3x0c3t BO4RD KEYBOARD");
    Serial.println("================================");

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

    // ========================================================
    // DIAGNOSTIC TFT
    // ========================================================

    Serial.println();
    Serial.println("[TFT] Configuration");

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
    // TEST VISUEL
    // ========================================================

    tft.fillScreen(
        COLOR_BLACK
    );

    tft.drawRect(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_WHITE
    );

    // Haut gauche
    tft.fillRect(
        0,
        0,
        20,
        20,
        COLOR_RED
    );

    // Haut droit
    tft.fillRect(
        SCREEN_WIDTH - 20,
        0,
        20,
        20,
        COLOR_GREEN
    );

    // Bas gauche
    tft.fillRect(
        0,
        SCREEN_HEIGHT - 20,
        20,
        20,
        COLOR_BLUE
    );

    // Bas droit
    tft.fillRect(
        SCREEN_WIDTH - 20,
        SCREEN_HEIGHT - 20,
        20,
        20,
        COLOR_YELLOW
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(
        35,
        30
    );

    tft.print(
        "TFT PORTRAIT"
    );

    tft.setCursor(
        35,
        55
    );

    tft.print(
        "ROTATION "
    );

    tft.print(
        SCREEN_ROTATION
    );

    tft.setCursor(
        35,
        80
    );

    tft.print(
        SCREEN_WIDTH
    );

    tft.print(
        " x "
    );

    tft.print(
        SCREEN_HEIGHT
    );

    delay(1200);

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
    // CALIBRATION DIRECTE
    // ========================================================

    Serial.println(
        "[TOUCH] Calibration automatique"
    );

    startTouchCalibration();

    // ========================================================
    // CLAVIER
    // ========================================================

    Serial.println(
        "[KEYBOARD] Initialisation"
    );

    keyboardInit();

    keyboardDraw();

    keyboardStarted = true;

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

    if (!keyboardStarted)
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

        delay(120);
    }
}