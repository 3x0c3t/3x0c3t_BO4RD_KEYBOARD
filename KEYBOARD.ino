#include <Arduino.h>

#include "config.h"
#include "touch_calibration.h"

// ============================================================
// VERSION
// ============================================================

#define KEYBOARD_VERSION "1.1"

// ============================================================
// TFT
// ============================================================

TFT_eSPI tft = TFT_eSPI();

// ============================================================
// ETAT DU PROGRAMME
// ============================================================

enum ProgramState
{
    STATE_INIT,
    STATE_CALIBRATION,
    STATE_KEYBOARD
};

static ProgramState programState =
    STATE_INIT;

// ============================================================
// PROTOTYPES
// ============================================================

void initTFT();
void initTouch();
void startKeyboard();
void drawKeyboard();

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(300);

    Serial.println();
    Serial.println("===============================");
    Serial.println("  3x0c3t BO4RD KEYBOARD");
    Serial.print("  VERSION ");
    Serial.println(KEYBOARD_VERSION);
    Serial.println("===============================");
    Serial.println();

    // ========================================================
    // TFT
    // ========================================================

    initTFT();

    // ========================================================
    // TOUCH
    // ========================================================

    initTouch();

    // ========================================================
    // DECISION CALIBRATION
    // ========================================================

#if FORCE_TOUCH_CALIBRATION

    Serial.println();
    Serial.println("[TOUCH] Calibration forcee");
    Serial.println("[TOUCH] EEPROM ignoree");
    Serial.println("[TOUCH] Lancement calibration");

    programState =
        STATE_CALIBRATION;

    startTouchCalibration();

#else

    Serial.println();
    Serial.println("[TOUCH] Recherche calibration EEPROM...");

    if (loadTouchCalibration())
    {
        Serial.println(
            "[TOUCH] Calibration EEPROM chargee"
        );

        Serial.print(
            "[TOUCH] Rotation = "
        );

        Serial.println(
            selectedTouchRotation()
        );

        programState =
            STATE_KEYBOARD;

        startKeyboard();
    }
    else
    {
        Serial.println(
            "[TOUCH] Aucune calibration valide"
        );

        Serial.println(
            "[TOUCH] Lancement calibration"
        );

        programState =
            STATE_CALIBRATION;

        startTouchCalibration();
    }

#endif
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
        programState ==
        STATE_CALIBRATION
    )
    {
        updateTouchCalibration();

        if (
            touchCalibrationFinished()
        )
        {
            Serial.println();
            Serial.println(
                "[TOUCH] Calibration terminee"
            );

            Serial.println(
                "[TOUCH] Calibration sauvegardee"
            );

            Serial.println(
                "[TOUCH] Passage au clavier"
            );

            programState =
                STATE_KEYBOARD;

            delay(500);

            startKeyboard();
        }

        yield();

        return;
    }

    // ========================================================
    // CLAVIER
    // ========================================================

    if (
        programState ==
        STATE_KEYBOARD
    )
    {
        // ----------------------------------------------------
        // Lecture clavier
        // ----------------------------------------------------

        uint16_t x = 0;
        uint16_t y = 0;

        if (
            tft.getTouch(
                &x,
                &y
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

            delay(150);

            while (
                tft.getTouch(
                    &x,
                    &y
                )
            )
            {
                delay(20);
                yield();
            }
        }

        yield();

        return;
    }

    yield();
}

// ============================================================
// INITIALISATION TFT
// ============================================================

void initTFT()
{
    Serial.println(
        "[TFT] Initialisation..."
    );

    tft.init();

    Serial.println(
        "[TFT] OK"
    );

    Serial.println();

    // --------------------------------------------------------
    // Rotation unique du projet
    // --------------------------------------------------------

    tft.setRotation(
        SCREEN_ROTATION
    );

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

    uint16_t realWidth =
        tft.width();

    uint16_t realHeight =
        tft.height();

    Serial.print(
        "[TFT] Taille reelle = "
    );

    Serial.print(
        realWidth
    );

    Serial.print(
        " x "
    );

    Serial.println(
        realHeight
    );

    if (
        realWidth ==
        SCREEN_WIDTH
        &&
        realHeight ==
        SCREEN_HEIGHT
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

    Serial.println();

    // --------------------------------------------------------
    // Fond initial
    // --------------------------------------------------------

    tft.fillScreen(
        COLOR_BLACK
    );
}

// ============================================================
// INITIALISATION TOUCH
// ============================================================

void initTouch()
{
    Serial.println(
        "[TOUCH] Initialisation..."
    );

    touchCalibrationInit();

    Serial.print(
        "[TOUCH] Rotation = "
    );

    Serial.println(
        SCREEN_ROTATION
    );

    Serial.print(
        "[TOUCH] Resolution = "
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

    Serial.println(
        "[TOUCH] OK"
    );

    Serial.println();
}

// ============================================================
// DEMARRAGE CLAVIER
// ============================================================

void startKeyboard()
{
    Serial.println();
    Serial.println(
        "[KEYBOARD] Initialisation..."
    );

    tft.setRotation(
        selectedTouchRotation()
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    drawKeyboard();

    Serial.println(
        "[KEYBOARD] OK"
    );

    Serial.println(
        "[KEYBOARD] Pret"
    );

    Serial.println();
}

// ============================================================
// CLAVIER
// ============================================================

void drawKeyboard()
{
    const int16_t margin = 5;

    const int16_t spacing = 4;

    const int16_t top = 35;

    const int16_t bottom = 315;

    const int16_t rows = 4;

    const int16_t cols = 3;

    const int16_t keyWidth =
        (
            SCREEN_WIDTH
            -
            (2 * margin)
            -
            ((cols - 1) * spacing)
        )
        /
        cols;

    const int16_t keyHeight =
        (
            bottom
            -
            top
            -
            ((rows - 1) * spacing)
        )
        /
        rows;

    // --------------------------------------------------------
    // TITRE
    // --------------------------------------------------------

    tft.setTextDatum(
        TC_DATUM
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(
        2
    );

    tft.drawString(
        "3x0c3t BO4RD",
        SCREEN_WIDTH / 2,
        10
    );

    // --------------------------------------------------------
    // TOUCHES
    // --------------------------------------------------------

    const char* keys[4][3] =
    {
        {
            "1",
            "2",
            "3"
        },
        {
            "4",
            "5",
            "6"
        },
        {
            "7",
            "8",
            "9"
        },
        {
            "*",
            "0",
            "#"
        }
    };

    for (
        int row = 0;
        row < rows;
        row++
    )
    {
        for (
            int col = 0;
            col < cols;
            col++
        )
        {
            int16_t x =
                margin
                +
                col *
                (
                    keyWidth +
                    spacing
                );

            int16_t y =
                top
                +
                row *
                (
                    keyHeight +
                    spacing
                );

            tft.drawRect(
                x,
                y,
                keyWidth,
                keyHeight,
                COLOR_WHITE
            );

            tft.setTextDatum(
                MC_DATUM
            );

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_BLACK
            );

            tft.setTextSize(
                3
            );

            tft.drawString(
                keys[row][col],
                x + keyWidth / 2,
                y + keyHeight / 2
            );
        }
    }

    tft.setTextDatum(
        TL_DATUM
    );
}