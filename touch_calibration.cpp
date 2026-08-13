#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "touch_calibration.h"


// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;


// ============================================================
// CALIBRATION
// ============================================================

uint16_t calData[5] =
{
    TOUCH_PREVIOUS_CAL_0,
    TOUCH_PREVIOUS_CAL_1,
    TOUCH_PREVIOUS_CAL_2,
    TOUCH_PREVIOUS_CAL_3,
    TOUCH_PREVIOUS_CAL_4
};


// ============================================================
// ECRAN DE CALIBRATION
// ============================================================

void drawCalibrationScreen()
{
    tft.fillScreen(COLOR_BLACK);

    tft.setTextDatum(MC_DATUM);


    // --------------------------------------------------------
    // Titre
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.drawString(
        "CALIBRATION",
        SCREEN_WIDTH / 2,
        25,
        2
    );


    // --------------------------------------------------------
    // Instructions
    // --------------------------------------------------------

    tft.drawString(
        "Touchez les croix",
        SCREEN_WIDTH / 2,
        50,
        2
    );

    tft.drawString(
        "entre leurs centres",
        SCREEN_WIDTH / 2,
        72,
        2
    );


    // --------------------------------------------------------
    // Information config.h
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_RED,
        COLOR_BLACK
    );

    tft.drawString(
        "Calibration desactivable",
        SCREEN_WIDTH / 2,
        100,
        2
    );

    tft.drawString(
        "dans config.h",
        SCREEN_WIDTH / 2,
        118,
        2
    );


    // --------------------------------------------------------
    // Bouton reprise anciennes valeurs
    // --------------------------------------------------------

    tft.fillRoundRect(
        TOUCH_REUSE_X,
        TOUCH_REUSE_Y,
        TOUCH_REUSE_W,
        TOUCH_REUSE_H,
        5,
        COLOR_DARK_GREY
    );

    tft.drawRoundRect(
        TOUCH_REUSE_X,
        TOUCH_REUSE_Y,
        TOUCH_REUSE_W,
        TOUCH_REUSE_H,
        5,
        COLOR_RED
    );


    tft.setTextColor(
        COLOR_RED,
        COLOR_DARK_GREY
    );

    tft.drawString(
        "CLIQUER POUR REPRENDRE LES",
        SCREEN_WIDTH / 2,
        TOUCH_REUSE_Y + 10,
        1
    );

    tft.drawString(
        "VALEURS PRECEDENTES",
        SCREEN_WIDTH / 2,
        TOUCH_REUSE_Y + 24,
        1
    );
}


// ============================================================
// TEST BOUTON REPRISE
// ============================================================

bool touchReusePreviousCalibration(
    int16_t x,
    int16_t y
)
{
    if (
        x >= TOUCH_REUSE_X &&
        x < TOUCH_REUSE_X + TOUCH_REUSE_W &&
        y >= TOUCH_REUSE_Y &&
        y < TOUCH_REUSE_Y + TOUCH_REUSE_H
    )
    {
        return true;
    }

    return false;
}


// ============================================================
// CHARGEMENT DES VALEURS PRECEDENTES
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
        "[TOUCH] Anciennes valeurs chargees"
    );

    Serial.print(
        "[TOUCH] CAL="
    );

    Serial.print(calData[0]);
    Serial.print(",");
    Serial.print(calData[1]);
    Serial.print(",");
    Serial.print(calData[2]);
    Serial.print(",");
    Serial.print(calData[3]);
    Serial.print(",");
    Serial.println(calData[4]);
}


// ============================================================
// CALIBRATION EFFECTIVE
// ============================================================

void touchCalibrate()
{
    Serial.println(
        "[TOUCH] Lancement calibration"
    );


    tft.calibrateTouch(
        calData,
        COLOR_RED,
        COLOR_BLACK,
        15
    );


    tft.setTouch(calData);


    Serial.println(
        "=== CALIBRATION TERMINEE ==="
    );

    Serial.print(
        "[TOUCH] CAL="
    );

    Serial.print(calData[0]);
    Serial.print(",");
    Serial.print(calData[1]);
    Serial.print(",");
    Serial.print(calData[2]);
    Serial.print(",");
    Serial.print(calData[3]);
    Serial.print(",");
    Serial.println(calData[4]);
}


// ============================================================
// POINT D'ENTREE UNIQUE
// ============================================================

void touchCalibration()
{
    // --------------------------------------------------------
    // Calibration désactivée
    // --------------------------------------------------------

    if (TOUCH_CALIBRATION == 0)
    {
        Serial.println(
            "[TOUCH] Calibration DESACTIVEE"
        );

        loadPreviousCalibration();

        return;
    }


    // --------------------------------------------------------
    // Calibration active
    // --------------------------------------------------------

    Serial.println(
        "[TOUCH] Calibration ACTIVE"
    );

    Serial.println(
        "[TOUCH] Preparation calibration"
    );


    // --------------------------------------------------------
    // Affichage
    // --------------------------------------------------------

    drawCalibrationScreen();


    delay(1000);


    // --------------------------------------------------------
    // Calibration
    // --------------------------------------------------------

    touchCalibrate();
}