#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// ETATS
// ============================================================

enum CalibrationState
{
    CALIBRATION_IDLE,

    CALIBRATION_CORNER_1,
    CALIBRATION_CORNER_2,
    CALIBRATION_CORNER_3,
    CALIBRATION_CORNER_4,

    CALIBRATION_ORIENTATION,

    CALIBRATION_DONE
};

static CalibrationState calibrationState =
    CALIBRATION_IDLE;

// ============================================================
// DONNEES RAW
// ============================================================

static uint16_t rawX[4] =
{
    0, 0, 0, 0
};

static uint16_t rawY[4] =
{
    0, 0, 0, 0
};

// ============================================================
// CALIBRATION TFT_eSPI
// ============================================================

static uint16_t calData[5] =
{
    0, 0, 0, 0, 0
};

// ============================================================
// ROTATION
// ============================================================

static uint8_t selectedRotation =
    SCREEN_ROTATION;

// ============================================================
// STABILISATION TOUCH
// ============================================================

static uint16_t stableX = 0;
static uint16_t stableY = 0;

static uint8_t stableCount = 0;

static bool wasTouched = false;

static uint32_t lastTouchTime = 0;

// ============================================================
// PARAMETRES
// ============================================================

static const uint32_t CALIBRATION_START_DELAY = 1200;

static const uint32_t TOUCH_DEBOUNCE_TIME = 300;

static const uint8_t REQUIRED_STABLE_SAMPLES = 4;

static const uint16_t STABLE_TOLERANCE = 80;

// ============================================================
// EEPROM
// ============================================================

struct TouchCalibrationData
{
    uint16_t magic;

    uint16_t version;

    uint16_t width;

    uint16_t height;

    uint16_t rotation;

    uint16_t calData[5];

    uint16_t checksum;
};

// ============================================================
// CHECKSUM
// ============================================================

static uint16_t calculateChecksum(
    const TouchCalibrationData& data
)
{
    uint32_t checksum = 0;

    checksum += data.magic;
    checksum += data.version;
    checksum += data.width;
    checksum += data.height;
    checksum += data.rotation;

    for (uint8_t i = 0; i < 5; i++)
    {
        checksum += data.calData[i];
    }

    return (uint16_t)(
        checksum & 0xFFFF
    );
}

// ============================================================
// TEXTE CENTRE
// ============================================================

static void drawCenteredText(
    const char* text,
    int16_t x,
    int16_t y,
    uint16_t color,
    uint8_t size
)
{
    tft.setTextDatum(
        MC_DATUM
    );

    tft.setTextColor(
        color,
        COLOR_BLACK
    );

    tft.setTextSize(
        size
    );

    tft.drawString(
        text,
        x,
        y
    );
}

// ============================================================
// CIBLE COIN
// ============================================================

static void drawCornerTarget(
    int16_t x,
    int16_t y
)
{
    const int16_t size = 12;

    tft.drawLine(
        x - size,
        y,
        x + size,
        y,
        COLOR_WHITE
    );

    tft.drawLine(
        x,
        y - size,
        x,
        y + size,
        COLOR_WHITE
    );

    tft.drawCircle(
        x,
        y,
        7,
        COLOR_RED
    );

    tft.fillCircle(
        x,
        y,
        2,
        COLOR_WHITE
    );
}

// ============================================================
// ECRAN CALIBRATION
// ============================================================

static void drawCalibrationScreen()
{
    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    // --------------------------------------------------------
    // CADRE
    // --------------------------------------------------------

    tft.drawRect(
        1,
        1,
        SCREEN_WIDTH - 2,
        SCREEN_HEIGHT - 2,
        COLOR_GREY
    );

    // --------------------------------------------------------
    // COINS
    //
    // IMPORTANT :
    // Les cibles sont dessinées AVANT le texte.
    // Le texte ne recouvre donc jamais les vrais coins.
    // --------------------------------------------------------

    const int16_t margin = 10;

    drawCornerTarget(
        margin,
        margin
    );

    drawCornerTarget(
        SCREEN_WIDTH - 1 - margin,
        margin
    );

    drawCornerTarget(
        SCREEN_WIDTH - 1 - margin,
        SCREEN_HEIGHT - 1 - margin
    );

    drawCornerTarget(
        margin,
        SCREEN_HEIGHT - 1 - margin
    );

    // --------------------------------------------------------
    // ZONE CENTRALE
    // --------------------------------------------------------

    tft.fillRect(
        25,
        105,
        SCREEN_WIDTH - 50,
        145,
        COLOR_BLACK
    );

    tft.drawRect(
        25,
        105,
        SCREEN_WIDTH - 50,
        145,
        COLOR_GREY
    );

    drawCenteredText(
        "3x0c3t_BO4RD",
        SCREEN_WIDTH / 2,
        130,
        COLOR_WHITE,
        2
    );

    drawCenteredText(
        "Calibration Affichage",
        SCREEN_WIDTH / 2,
        157,
        COLOR_CYAN,
        1
    );

    drawCenteredText(
        "& Tactile",
        SCREEN_WIDTH / 2,
        174,
        COLOR_CYAN,
        1
    );

    // --------------------------------------------------------
    // INSTRUCTION
    // --------------------------------------------------------

    const char* instruction =
        "";

    const char* counter =
        "";

    switch (calibrationState)
    {
        case CALIBRATION_CORNER_1:

            instruction =
                "Toucher HAUT GAUCHE";

            counter =
                "1 / 4";

            break;

        case CALIBRATION_CORNER_2:

            instruction =
                "Toucher HAUT DROIT";

            counter =
                "2 / 4";

            break;

        case CALIBRATION_CORNER_3:

            instruction =
                "Toucher BAS DROIT";

            counter =
                "3 / 4";

            break;

        case CALIBRATION_CORNER_4:

            instruction =
                "Toucher BAS GAUCHE";

            counter =
                "4 / 4";

            break;

        default:

            break;
    }

    drawCenteredText(
        instruction,
        SCREEN_WIDTH / 2,
        205,
        COLOR_YELLOW,
        1
    );

    drawCenteredText(
        counter,
        SCREEN_WIDTH / 2,
        230,
        COLOR_GREEN,
        2
    );

    tft.setTextDatum(
        TL_DATUM
    );
}

// ============================================================
// OK NORMAL
// ============================================================

static void drawOKNormal()
{
    tft.drawRect(
        35,
        105,
        SCREEN_WIDTH - 70,
        80,
        COLOR_GREEN
    );

    drawCenteredText(
        "OK",
        SCREEN_WIDTH / 2,
        135,
        COLOR_GREEN,
        4
    );

    drawCenteredText(
        "ORIENTATION 0",
        SCREEN_WIDTH / 2,
        168,
        COLOR_WHITE,
        1
    );
}

// ============================================================
// OK RETOURNE
//
// Pas de Sprite.
// On dessine simplement le texte caractère par caractère
// à l'envers pour éviter les problèmes TFT_eSPI Sprite.
// ============================================================

static void drawOKInverted()
{
    const int16_t centerX =
        SCREEN_WIDTH / 2;

    const int16_t centerY =
        255;

    tft.drawRect(
        35,
        215,
        SCREEN_WIDTH - 70,
        85,
        COLOR_YELLOW
    );

    // --------------------------------------------------------
    // "OK" retourné de 180°.
    //
    // Utilisation de deux rectangles et lignes :
    // on veut ici uniquement permettre à l'utilisateur
    // d'identifier le sens de lecture.
    // --------------------------------------------------------

    const int16_t cx =
        centerX;

    const int16_t cy =
        centerY;

    // O retourné
    tft.drawCircle(
        cx - 18,
        cy,
        13,
        COLOR_YELLOW
    );

    // K retourné
    tft.drawLine(
        cx + 4,
        cy - 14,
        cx + 4,
        cy + 14,
        COLOR_YELLOW
    );

    tft.drawLine(
        cx + 4,
        cy,
        cx + 19,
        cy - 14,
        COLOR_YELLOW
    );

    tft.drawLine(
        cx + 4,
        cy,
        cx + 19,
        cy + 14,
        COLOR_YELLOW
    );

    drawCenteredText(
        "ORIENTATION 2",
        SCREEN_WIDTH / 2,
        285,
        COLOR_WHITE,
        1
    );
}

// ============================================================
// ECRAN ORIENTATION
// ============================================================

static void drawOrientationScreen()
{
    // --------------------------------------------------------
    // IMPORTANT :
    // On garde la rotation utilisée pendant la calibration.
    // On ne change PAS la rotation avant le choix utilisateur.
    // --------------------------------------------------------

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    // --------------------------------------------------------
    // CADRE
    // --------------------------------------------------------

    tft.drawRect(
        2,
        2,
        SCREEN_WIDTH - 4,
        SCREEN_HEIGHT - 4,
        COLOR_WHITE
    );

    // --------------------------------------------------------
    // TITRE
    // --------------------------------------------------------

    drawCenteredText(
        "Choisir l'orientation",
        SCREEN_WIDTH / 2,
        35,
        COLOR_WHITE,
        2
    );

    drawCenteredText(
        "Toucher le OK correspondant",
        SCREEN_WIDTH / 2,
        65,
        COLOR_CYAN,
        1
    );

    // --------------------------------------------------------
    // ORIENTATION 0
    // --------------------------------------------------------

    drawOKNormal();

    // --------------------------------------------------------
    // SEPARATION
    // --------------------------------------------------------

    tft.drawLine(
        15,
        195,
        SCREEN_WIDTH - 15,
        195,
        COLOR_GREY
    );

    // --------------------------------------------------------
    // ORIENTATION 2
    // --------------------------------------------------------

    drawOKInverted();

    tft.setTextDatum(
        TL_DATUM
    );
}

// ============================================================
// ECRAN FIN
// ============================================================

static void drawCalibrationFinishedScreen()
{
    tft.setRotation(
        selectedRotation
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    tft.drawRect(
        4,
        4,
        SCREEN_WIDTH - 8,
        SCREEN_HEIGHT - 8,
        COLOR_GREEN
    );

    drawCenteredText(
        "Calibration OK",
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2 - 25,
        COLOR_GREEN,
        2
    );

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "Rotation %u",
        selectedRotation
    );

    drawCenteredText(
        buffer,
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2 + 15,
        COLOR_WHITE,
        2
    );

    tft.setTextDatum(
        TL_DATUM
    );

    delay(800);
}

// ============================================================
// CALCUL CALIBRATION
// ============================================================

static bool calculateTouchCalibration()
{
    Serial.println();
    Serial.println(
        "[TOUCH] Calcul calibration..."
    );

    for (uint8_t i = 0; i < 4; i++)
    {
        Serial.print(
            "[TOUCH] Coin "
        );

        Serial.print(
            i + 1
        );

        Serial.print(
            " RAW X="
        );

        Serial.print(
            rawX[i]
        );

        Serial.print(
            " Y="
        );

        Serial.println(
            rawY[i]
        );
    }

    // --------------------------------------------------------
    // X
    // --------------------------------------------------------

    int32_t x0 =
        (
            (int32_t)rawX[0] +
            (int32_t)rawX[3]
        ) / 2;

    int32_t x1 =
        (
            (int32_t)rawX[1] +
            (int32_t)rawX[2]
        ) / 2;

    // --------------------------------------------------------
    // Y
    // --------------------------------------------------------

    int32_t y0 =
        (
            (int32_t)rawY[0] +
            (int32_t)rawY[1]
        ) / 2;

    int32_t y1 =
        (
            (int32_t)rawY[2] +
            (int32_t)rawY[3]
        ) / 2;

    // --------------------------------------------------------
    // Inversion éventuelle des axes
    // --------------------------------------------------------

    if (x0 > x1)
    {
        int32_t tmp =
            x0;

        x0 =
            x1;

        x1 =
            tmp;
    }

    if (y0 > y1)
    {
        int32_t tmp =
            y0;

        y0 =
            y1;

        y1 =
            tmp;
    }

    // --------------------------------------------------------
    // Vérification
    // --------------------------------------------------------

    if (
        x1 <= x0 ||
        y1 <= y0
    )
    {
        Serial.println(
            "[TOUCH] ERREUR calibration invalide"
        );

        return false;
    }

    // --------------------------------------------------------
    // TFT_eSPI attend les bornes min/max RAW.
    // PAS une largeur/hauteur.
    // --------------------------------------------------------

    calData[0] =
        (uint16_t)x0;

    calData[1] =
        (uint16_t)x1;

    calData[2] =
        (uint16_t)y0;

    calData[3] =
        (uint16_t)y1;

    calData[4] =
        0;

    tft.setTouch(
        calData
    );

    Serial.println(
        "[TOUCH] Parametres appliques"
    );

    Serial.print(
        "[TOUCH] X MIN = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "[TOUCH] X MAX = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "[TOUCH] Y MIN = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "[TOUCH] Y MAX = "
    );

    Serial.println(
        calData[3]
    );

    return true;
}

// ============================================================
// INIT
// ============================================================

void touchCalibrationInit()
{
    tft.setRotation(
        SCREEN_ROTATION
    );

    Serial.println(
        "[TOUCH] Calibration Init"
    );

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
}

// ============================================================
// START
// ============================================================

void startTouchCalibration()
{
    calibrationState =
        CALIBRATION_CORNER_1;

    selectedRotation =
        SCREEN_ROTATION;

    stableX =
        0;

    stableY =
        0;

    stableCount =
        0;

    wasTouched =
        false;

    lastTouchTime =
        0;

    for (uint8_t i = 0; i < 4; i++)
    {
        rawX[i] =
            0;

        rawY[i] =
            0;
    }

    for (uint8_t i = 0; i < 5; i++)
    {
        calData[i] =
            0;
    }

    // --------------------------------------------------------
    // AFFICHER D'ABORD
    // --------------------------------------------------------

    drawCalibrationScreen();

    // --------------------------------------------------------
    // IMPORTANT :
    // aucune lecture tactile pendant cette période.
    // Cela évite que le démarrage du XPT2046 soit pris
    // pour un appui.
    // --------------------------------------------------------

    delay(
        CALIBRATION_START_DELAY
    );

    stableCount =
        0;

    wasTouched =
        false;

    lastTouchTime =
        millis();

    Serial.println();

    Serial.println(
        "[TOUCH] Calibration tactile"
    );

    Serial.println(
        "[TOUCH] Attente toucher coin 1"
    );
}

// ============================================================
// LECTURE RAW STABLE
// ============================================================

static bool readStableRawTouch(
    uint16_t& x,
    uint16_t& y
)
{
    uint16_t currentX =
        0;

    uint16_t currentY =
        0;

    bool pressed =
        tft.getTouchRaw(
            &currentX,
            &currentY
        );

    if (!pressed)
    {
        stableCount =
            0;

        wasTouched =
            false;

        return false;
    }

    // --------------------------------------------------------
    // Eviter une répétition du même appui
    // --------------------------------------------------------

    if (
        wasTouched
        &&
        (
            millis() -
            lastTouchTime
        ) <
        TOUCH_DEBOUNCE_TIME
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // Première mesure
    // --------------------------------------------------------

    if (
        stableCount ==
        0
    )
    {
        stableX =
            currentX;

        stableY =
            currentY;

        stableCount =
            1;

        return false;
    }

    // --------------------------------------------------------
    // Vérification stabilité
    // --------------------------------------------------------

    if (
        abs(
            (int32_t)currentX -
            (int32_t)stableX
        ) <=
        STABLE_TOLERANCE
        &&
        abs(
            (int32_t)currentY -
            (int32_t)stableY
        ) <=
        STABLE_TOLERANCE
    )
    {
        if (
            stableCount <
            REQUIRED_STABLE_SAMPLES
        )
        {
            stableCount++;
        }
    }
    else
    {
        stableX =
            currentX;

        stableY =
            currentY;

        stableCount =
            1;

        return false;
    }

    // --------------------------------------------------------
    // Toucher confirmé
    // --------------------------------------------------------

    if (
        stableCount >=
        REQUIRED_STABLE_SAMPLES
    )
    {
        x =
            currentX;

        y =
            currentY;

        stableCount =
            0;

        wasTouched =
            true;

        lastTouchTime =
            millis();

        Serial.print(
            "[TOUCH RAW] X="
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

        return true;
    }

    return false;
}

// ============================================================
// LECTURE CALIBREE
// ============================================================

static bool readCalibratedTouch(
    uint16_t& x,
    uint16_t& y
)
{
    uint16_t currentX =
        0;

    uint16_t currentY =
        0;

    bool pressed =
        tft.getTouch(
            &currentX,
            &currentY
        );

    if (!pressed)
    {
        return false;
    }

    if (
        (
            millis() -
            lastTouchTime
        ) <
        TOUCH_DEBOUNCE_TIME
    )
    {
        return false;
    }

    x =
        currentX;

    y =
        currentY;

    lastTouchTime =
        millis();

    Serial.print(
        "[TOUCH CAL] X="
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

    return true;
}

// ============================================================
// UPDATE
// ============================================================

void updateTouchCalibration()
{
    // ========================================================
    // ETAPE 1 : COINS
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_CORNER_1
        ||
        calibrationState ==
        CALIBRATION_CORNER_2
        ||
        calibrationState ==
        CALIBRATION_CORNER_3
        ||
        calibrationState ==
        CALIBRATION_CORNER_4
    )
    {
        uint16_t x =
            0;

        uint16_t y =
            0;

        if (
            !readStableRawTouch(
                x,
                y
            )
        )
        {
            return;
        }

        uint8_t index =
            0;

        if (
            calibrationState ==
            CALIBRATION_CORNER_1
        )
        {
            index =
                0;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_2
        )
        {
            index =
                1;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_3
        )
        {
            index =
                2;
        }
        else
        {
            index =
                3;
        }

        rawX[index] =
            x;

        rawY[index] =
            y;

        Serial.print(
            "[TOUCH] Coin "
        );

        Serial.print(
            index + 1
        );

        Serial.println(
            " valide"
        );

        // ----------------------------------------------------
        // PASSAGE ETAPE SUIVANTE
        // ----------------------------------------------------

        if (
            calibrationState ==
            CALIBRATION_CORNER_1
        )
        {
            calibrationState =
                CALIBRATION_CORNER_2;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_2
        )
        {
            calibrationState =
                CALIBRATION_CORNER_3;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_3
        )
        {
            calibrationState =
                CALIBRATION_CORNER_4;
        }
        else
        {
            // ------------------------------------------------
            // 4 coins terminés
            // ------------------------------------------------

            if (
                !calculateTouchCalibration()
            )
            {
                calibrationState =
                    CALIBRATION_CORNER_1;

                drawCalibrationScreen();

                return;
            }

            calibrationState =
                CALIBRATION_ORIENTATION;

            // ------------------------------------------------
            // IMPORTANT :
            // afficher l'écran AVANT toute lecture.
            // ------------------------------------------------

            drawOrientationScreen();

            delay(400);

            stableCount =
                0;

            wasTouched =
                false;

            lastTouchTime =
                millis();

            Serial.println();

            Serial.println(
                "[TOUCH] 4 coins termines"
            );

            Serial.println(
                "[TOUCH] Choix orientation 0 ou 2"
            );

            return;
        }

        // ----------------------------------------------------
        // Redessiner la cible suivante
        // ----------------------------------------------------

        drawCalibrationScreen();

        delay(120);

        stableCount =
            0;

        wasTouched =
            false;

        return;
    }

    // ========================================================
    // ETAPE 2 : ORIENTATION
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_ORIENTATION
    )
    {
        uint16_t x =
            0;

        uint16_t y =
            0;

        if (
            !readCalibratedTouch(
                x,
                y
            )
        )
        {
            return;
        }

        Serial.print(
            "[ORIENTATION] Touch X="
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

        // ----------------------------------------------------
        // Zone ORIENTATION 0
        // ----------------------------------------------------

        if (
            x >= 35 &&
            x <= SCREEN_WIDTH - 35 &&
            y >= 105 &&
            y <= 185
        )
        {
            selectedRotation =
                0;

            Serial.println(
                "[ORIENTATION] Rotation 0 selectionnee"
            );
        }

        // ----------------------------------------------------
        // Zone ORIENTATION 2
        // ----------------------------------------------------

        else if (
            x >= 35 &&
            x <= SCREEN_WIDTH - 35 &&
            y >= 215 &&
            y <= 300
        )
        {
            selectedRotation =
                2;

            Serial.println(
                "[ORIENTATION] Rotation 2 selectionnee"
            );
        }

        else
        {
            Serial.println(
                "[ORIENTATION] Touch hors zone"
            );

            return;
        }

        // ----------------------------------------------------
        // Appliquer
        // ----------------------------------------------------

        tft.setTouch(
            calData
        );

        tft.setRotation(
            selectedRotation
        );

        // ----------------------------------------------------
        // Sauvegarde EEPROM
        // ----------------------------------------------------

        if (
            saveTouchCalibration()
        )
        {
            Serial.println(
                "[TOUCH] Calibration sauvegardee EEPROM"
            );
        }
        else
        {
            Serial.println(
                "[TOUCH] ERREUR sauvegarde EEPROM"
            );
        }

        calibrationState =
            CALIBRATION_DONE;

        drawCalibrationFinishedScreen();

        return;
    }
}

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive()
{
    return (
        calibrationState !=
        CALIBRATION_IDLE
        &&
        calibrationState !=
        CALIBRATION_DONE
    );
}

bool touchCalibrationFinished()
{
    return (
        calibrationState ==
        CALIBRATION_DONE
    );
}

// ============================================================
// ROTATION SELECTIONNEE
// ============================================================

uint8_t selectedTouchRotation()
{
    return selectedRotation;
}

// ============================================================
// EEPROM LOAD
// ============================================================

bool loadTouchCalibration()
{
    TouchCalibrationData data;

    EEPROM.begin(
        TOUCH_EEPROM_SIZE
    );

    EEPROM.get(
        TOUCH_EEPROM_ADDRESS,
        data
    );

    EEPROM.end();

    if (
        data.magic !=
        TOUCH_EEPROM_MAGIC
    )
    {
        return false;
    }

    if (
        data.version !=
        TOUCH_EEPROM_VERSION
    )
    {
        return false;
    }

    if (
        data.width !=
        SCREEN_WIDTH
        ||
        data.height !=
        SCREEN_HEIGHT
    )
    {
        return false;
    }

    if (
        data.rotation != 0
        &&
        data.rotation != 2
    )
    {
        return false;
    }

    if (
        data.checksum !=
        calculateChecksum(data)
    )
    {
        return false;
    }

    for (uint8_t i = 0; i < 5; i++)
    {
        calData[i] =
            data.calData[i];
    }

    selectedRotation =
        data.rotation;

    tft.setTouch(
        calData
    );

    tft.setRotation(
        selectedRotation
    );

    return true;
}

// ============================================================
// EEPROM SAVE
// ============================================================

bool saveTouchCalibration()
{
    TouchCalibrationData data;

    data.magic =
        TOUCH_EEPROM_MAGIC;

    data.version =
        TOUCH_EEPROM_VERSION;

    data.width =
        SCREEN_WIDTH;

    data.height =
        SCREEN_HEIGHT;

    data.rotation =
        selectedRotation;

    for (uint8_t i = 0; i < 5; i++)
    {
        data.calData[i] =
            calData[i];
    }

    data.checksum =
        calculateChecksum(
            data
        );

    EEPROM.begin(
        TOUCH_EEPROM_SIZE
    );

    EEPROM.put(
        TOUCH_EEPROM_ADDRESS,
        data
    );

    bool result =
        EEPROM.commit();

    EEPROM.end();

    return result;
}

// ============================================================
// EEPROM CLEAR
// ============================================================

bool clearTouchCalibration()
{
    EEPROM.begin(
        TOUCH_EEPROM_SIZE
    );

    for (
        uint16_t i = 0;
        i < TOUCH_EEPROM_SIZE;
        i++
    )
    {
        EEPROM.write(
            i,
            0xFF
        );
    }

    bool result =
        EEPROM.commit();

    EEPROM.end();

    return result;
}

// ============================================================
// COMPATIBILITE
// ============================================================

bool touchCalibrationChoice()
{
    return (
        calibrationState ==
        CALIBRATION_ORIENTATION
    );
}

bool loadPreviousCalibration()
{
    return loadTouchCalibration();
}