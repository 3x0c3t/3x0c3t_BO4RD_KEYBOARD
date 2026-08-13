#include "touch_calibration.h"
#include "config.h"

static uint16_t calData[5] = {
    TOUCH_PREVIOUS_CAL_0,
    TOUCH_PREVIOUS_CAL_1,
    TOUCH_PREVIOUS_CAL_2,
    TOUCH_PREVIOUS_CAL_3,
    TOUCH_PREVIOUS_CAL_4
};


// ============================================================
// Charger immédiatement la calibration précédente
// ============================================================

void loadPreviousCalibration()
{
    calData[0] = TOUCH_PREVIOUS_CAL_0;
    calData[1] = TOUCH_PREVIOUS_CAL_1;
    calData[2] = TOUCH_PREVIOUS_CAL_2;
    calData[3] = TOUCH_PREVIOUS_CAL_3;
    calData[4] = TOUCH_PREVIOUS_CAL_4;

    tft.setTouch(calData);

    Serial.println(
        "[TOUCH] Configuration precedente chargee"
    );

    Serial.print("[TOUCH] CAL0=");
    Serial.print(calData[0]);

    Serial.print(" CAL1=");
    Serial.print(calData[1]);

    Serial.print(" CAL2=");
    Serial.print(calData[2]);

    Serial.print(" CAL3=");
    Serial.print(calData[3]);

    Serial.print(" CAL4=");
    Serial.println(calData[4]);
}


// ============================================================
// Dessiner bouton
// ============================================================

static void drawButton(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    uint16_t fill,
    uint16_t border,
    const char *line1,
    const char *line2
)
{
    tft.fillRoundRect(
        x,
        y,
        w,
        h,
        6,
        fill
    );

    tft.drawRoundRect(
        x,
        y,
        w,
        h,
        6,
        border
    );

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        fill
    );

    tft.drawString(
        line1,
        x + (w / 2),
        y + 22,
        2
    );

    if (line2 != nullptr)
    {
        tft.drawString(
            line2,
            x + (w / 2),
            y + 45,
            2
        );
    }

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// Écran de choix
// ============================================================

void touchCalibrationScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.drawString(
        "CALIBRATION TACTILE",
        SCREEN_WIDTH / 2,
        30,
        2
    );

    tft.setTextColor(
        COLOR_GREY,
        COLOR_BLACK
    );

    tft.drawString(
        "Choisissez une option",
        SCREEN_WIDTH / 2,
        58,
        2
    );


    // --------------------------------------------------------
    // CALIBRATION
    // --------------------------------------------------------

    drawButton(
        TOUCH_CAL_BUTTON_X,
        TOUCH_CAL_BUTTON_Y,
        TOUCH_CAL_BUTTON_W,
        TOUCH_CAL_BUTTON_H,
        COLOR_BLUE,
        COLOR_CYAN,
        "CALIBRATION",
        "4 ANGLES"
    );


    // --------------------------------------------------------
    // CONFIG PRECEDENTE
    // --------------------------------------------------------

    drawButton(
        TOUCH_REUSE_BUTTON_X,
        TOUCH_REUSE_BUTTON_Y,
        TOUCH_REUSE_BUTTON_W,
        TOUCH_REUSE_BUTTON_H,
        COLOR_DARK_GREY,
        COLOR_WHITE,
        "CONFIG PRECEDENTE",
        "PASSER AU CLAVIER"
    );


    // --------------------------------------------------------
    // Information
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_GREY,
        COLOR_BLACK
    );

    tft.drawString(
        "Valeurs actuelles deja chargees",
        SCREEN_WIDTH / 2,
        295,
        1
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// Test bouton calibration
// ============================================================

static bool isCalibrationButton(
    uint16_t x,
    uint16_t y
)
{
    return (
        x >= TOUCH_CAL_BUTTON_X &&
        x < TOUCH_CAL_BUTTON_X + TOUCH_CAL_BUTTON_W &&
        y >= TOUCH_CAL_BUTTON_Y &&
        y < TOUCH_CAL_BUTTON_Y + TOUCH_CAL_BUTTON_H
    );
}


// ============================================================
// Test bouton config précédente
// ============================================================

static bool isReuseButton(
    uint16_t x,
    uint16_t y
)
{
    return (
        x >= TOUCH_REUSE_BUTTON_X &&
        x < TOUCH_REUSE_BUTTON_X + TOUCH_REUSE_BUTTON_W &&
        y >= TOUCH_REUSE_BUTTON_Y &&
        y < TOUCH_REUSE_BUTTON_Y + TOUCH_REUSE_BUTTON_H
    );
}


// ============================================================
// Fonction publique
// ============================================================

bool touchReusePreviousCalibration()
{
    return false;
}


// ============================================================
// Calibration 4 angles
// ============================================================

static void runCalibration()
{
    Serial.println(
        "=== CALIBRATION 4 ANGLES ==="
    );

    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        35,
        2
    );

    tft.drawString(
        "Touchez les 4 cibles",
        SCREEN_WIDTH / 2,
        60,
        2
    );

    tft.drawString(
        "dans leurs centres",
        SCREEN_WIDTH / 2,
        82,
        2
    );

    tft.setTextColor(
        COLOR_RED,
        COLOR_BLACK
    );

    tft.drawString(
        "CERCLE + CROIX",
        SCREEN_WIDTH / 2,
        108,
        2
    );

    tft.setTextDatum(TL_DATUM);

    delay(1000);


    // --------------------------------------------------------
    // Calibration TFT_eSPI
    // --------------------------------------------------------

    tft.calibrateTouch(
        calData,
        COLOR_RED,
        COLOR_BLACK,
        15
    );


    // --------------------------------------------------------
    // Appliquer immédiatement
    // --------------------------------------------------------

    tft.setTouch(calData);


    // --------------------------------------------------------
    // Logs
    // --------------------------------------------------------

    Serial.println(
        "=== CALIBRATION TERMINEE ==="
    );

    Serial.print("CAL0=");
    Serial.print(calData[0]);

    Serial.print(" CAL1=");
    Serial.print(calData[1]);

    Serial.print(" CAL2=");
    Serial.print(calData[2]);

    Serial.print(" CAL3=");
    Serial.print(calData[3]);

    Serial.print(" CAL4=");
    Serial.println(calData[4]);

    delay(500);
}


// ============================================================
// Choix calibration
// ============================================================

void touchCalibration()
{
#if TOUCH_CALIBRATION

    Serial.println(
        "[TOUCH] Calibration automatique ACTIVE"
    );

    // --------------------------------------------------------
    // IMPORTANT
    //
    // On charge les anciennes valeurs AVANT de demander
    // à l'utilisateur de toucher un bouton.
    // --------------------------------------------------------

    loadPreviousCalibration();

    touchCalibrationScreen();

    Serial.println(
        "[TOUCH] Attente choix utilisateur"
    );


    while (true)
    {
        uint16_t x = 0;
        uint16_t y = 0;


        // ----------------------------------------------------
        // Lecture tactile
        // ----------------------------------------------------

        if (tft.getTouch(&x, &y))
        {
            Serial.print(
                "[TOUCH SELECT] X="
            );

            Serial.print(x);

            Serial.print(
                " Y="
            );

            Serial.println(y);


            // ------------------------------------------------
            // Bouton calibration
            // ------------------------------------------------

            if (isCalibrationButton(x, y))
            {
                Serial.println(
                    "[TOUCH] CHOIX = CALIBRATION"
                );

                delay(300);

                runCalibration();

                return;
            }


            // ------------------------------------------------
            // Bouton configuration précédente
            // ------------------------------------------------

            if (isReuseButton(x, y))
            {
                Serial.println(
                    "[TOUCH] CHOIX = CONFIG PRECEDENTE"
                );

                Serial.println(
                    "[TOUCH] Passage au clavier"
                );

                delay(300);

                return;
            }


            Serial.println(
                "[TOUCH] Contact hors bouton"
            );

            delay(200);
        }

        delay(10);
    }

#else

    Serial.println(
        "[TOUCH] Calibration desactivee"
    );

    loadPreviousCalibration();

#endif
}