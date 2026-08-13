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
    delay(500);

    Serial.println();
    Serial.println();
    Serial.println("================================");
    Serial.println("  3x0c3t BO4RD KEYBOARD v1.0");
    Serial.println("================================");

    // --------------------------------------------------------
    // TFT
    // --------------------------------------------------------

    Serial.println("[TFT] Initialisation...");

    tft.init();

    // Rotation UNIQUE du projet
    tft.setRotation(SCREEN_ROTATION);

    tft.fillScreen(COLOR_BLACK);

    Serial.println("[TFT] Initialisation OK");

    // --------------------------------------------------------
    // DIAGNOSTIC RESOLUTION REELLE
    // --------------------------------------------------------

    Serial.println();
    Serial.println("[TFT] DIAGNOSTIC");

    Serial.print("[TFT] Rotation demandee = ");
    Serial.println(SCREEN_ROTATION);

    Serial.print("[TFT] Largeur reelle = ");
    Serial.println(tft.width());

    Serial.print("[TFT] Hauteur reelle = ");
    Serial.println(tft.height());

    Serial.print("[TFT] Configuration = ");
    Serial.print(SCREEN_WIDTH);
    Serial.print(" x ");
    Serial.println(SCREEN_HEIGHT);

    if (
        tft.width() == SCREEN_WIDTH &&
        tft.height() == SCREEN_HEIGHT
    )
    {
        Serial.println("[TFT] RESOLUTION = OK");
    }
    else
    {
        Serial.println("[TFT] RESOLUTION = ERREUR");
        Serial.println("[TFT] La configuration TFT_eSPI ne correspond pas");
    }

    // --------------------------------------------------------
    // TEST VISUEL PORTRAIT
    // --------------------------------------------------------

    tft.fillScreen(COLOR_BLACK);

    tft.drawRect(
        0,
        0,
        tft.width(),
        tft.height(),
        COLOR_WHITE
    );

    tft.fillRect(
        0,
        0,
        20,
        20,
        COLOR_RED
    );

    tft.fillRect(
        tft.width() - 20,
        0,
        20,
        20,
        COLOR_GREEN
    );

    tft.fillRect(
        0,
        tft.height() - 20,
        20,
        20,
        COLOR_BLUE
    );

    tft.fillRect(
        tft.width() - 20,
        tft.height() - 20,
        20,
        20,
        COLOR_YELLOW
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(35, 30);
    tft.print("TFT PORTRAIT");

    tft.setCursor(35, 55);
    tft.print("ROTATION = ");
    tft.print(SCREEN_ROTATION);

    tft.setCursor(35, 80);
    tft.print(tft.width());
    tft.print(" x ");
    tft.print(tft.height());

    delay(1500);

    // --------------------------------------------------------
    // TOUCH
    // --------------------------------------------------------

    Serial.println();
    Serial.println("[TOUCH] Initialisation");

    touchCalibrationInit();

    Serial.println("[TOUCH] Initialisation OK");

    // --------------------------------------------------------
    // CHOIX CALIBRATION
    // --------------------------------------------------------

    Serial.println("[TOUCH] Ecran choix calibration");

    touchCalibrationChoice();

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
    // CALIBRATION / CHOIX
    // ========================================================

    if (touchCalibrationActive())
    {
        if (
            tft.getTouch(
                &x,
                &y,
                TOUCH_THRESHOLD
            )
        )
        {
            Serial.print("[TOUCH] X=");
            Serial.print(x);

            Serial.print(" Y=");
            Serial.println(y);

            if (touchCalibrationUpdate(x, y))
            {
                Serial.println(
                    "[TOUCH] Configuration terminee"
                );

                keyboardInit();
                keyboardDraw();

                keyboardStarted = true;
            }

            delay(150);
        }

        return;
    }

    // ========================================================
    // CLAVIER
    // ========================================================

    if (!keyboardStarted)
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
        Serial.print("[TOUCH] X=");
        Serial.print(x);

        Serial.print(" Y=");
        Serial.println(y);

        keyboardUpdate(
            (int16_t)x,
            (int16_t)y
        );

        delay(150);
    }
}