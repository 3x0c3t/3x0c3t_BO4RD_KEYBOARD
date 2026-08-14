#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// ETATS CALIBRATION
// ============================================================

enum CalibrationState
{
    CALIBRATION_IDLE,

    CALIBRATION_WAIT_RELEASE,

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
// RAW
// ============================================================

static uint16_t rawX[4] =
{
    0,
    0,
    0,
    0
};

static uint16_t rawY[4] =
{
    0,
    0,
    0,
    0
};

// ============================================================
// CALIBRATION TFT_eSPI
// ============================================================

static uint16_t calData[5] =
{
    0,
    0,
    0,
    0,
    0
};

// ============================================================
// ORIENTATION
// ============================================================

static uint8_t selectedRotation =
    SCREEN_ROTATION;

// ============================================================
// TOUCH
// ============================================================

static bool touchPressed =
    false;

static uint32_t calibrationStartTime =
    0;

static uint32_t lastTouchTime =
    0;

static uint16_t stableX =
    0;

static uint16_t stableY =
    0;

static uint8_t stableCount =
    0;

// ============================================================
// PARAMETRES
// ============================================================

static const uint32_t CALIBRATION_START_DELAY =
    1200;

static const uint32_t TOUCH_DEBOUNCE_TIME =
    300;

static const uint8_t REQUIRED_STABLE_SAMPLES =
    4;

static const uint16_t STABLE_TOLERANCE =
    80;

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

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        checksum += data.calData[i];
    }

    return (
        uint16_t
    )(
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
    const int16_t size =
        11;

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
        6,
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

    // --------------------------------------------------------
    // EFFACEMENT
    // --------------------------------------------------------

    tft.fillScreen(
        COLOR_BLACK
    );

    // --------------------------------------------------------
    // COORDONNEES REELLES DES COINS
    // --------------------------------------------------------

    const int16_t margin =
        8;

    const int16_t left =
        margin;

    const int16_t right =
        SCREEN_WIDTH - 1 - margin;

    const int16_t top =
        margin;

    const int16_t bottom =
        SCREEN_HEIGHT - 1 - margin;

    // --------------------------------------------------------
    // LES COINS SONT DESSINES EN PREMIER
    // --------------------------------------------------------

    drawCornerTarget(
        left,
        top
    );

    drawCornerTarget(
        right,
        top
    );

    drawCornerTarget(
        right,
        bottom
    );

    drawCornerTarget(
        left,
        bottom
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

    // ========================================================
    // TEXTE CENTRAL
    // ========================================================

    drawCenteredText(
        "3x0c3t_BO4RD",
        SCREEN_WIDTH / 2,
        120,
        COLOR_WHITE,
        2
    );

    drawCenteredText(
        "Calibration Affichage",
        SCREEN_WIDTH / 2,
        150,
        COLOR_CYAN,
        1
    );

    drawCenteredText(
        "& Tactile",
        SCREEN_WIDTH / 2,
        168,
        COLOR_CYAN,
        1
    );

    // ========================================================
    // INSTRUCTION
    // ========================================================

    const char* instruction =
        "";

    switch (
        calibrationState
    )
    {
        case CALIBRATION_WAIT_RELEASE:

            instruction =
                "Relacher l'ecran";

            break;

        case CALIBRATION_CORNER_1:

            instruction =
                "Toucher HAUT GAUCHE";

            break;

        case CALIBRATION_CORNER_2:

            instruction =
                "Toucher HAUT DROIT";

            break;

        case CALIBRATION_CORNER_3:

            instruction =
                "Toucher BAS DROIT";

            break;

        case CALIBRATION_CORNER_4:

            instruction =
                "Toucher BAS GAUCHE";

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

    // ========================================================
    // COMPTEUR
    // ========================================================

    const char* counter =
        "";

    switch (
        calibrationState
    )
    {
        case CALIBRATION_WAIT_RELEASE:

            counter =
                "ATTENTE";

            break;

        case CALIBRATION_CORNER_1:

            counter =
                "1 / 4";

            break;

        case CALIBRATION_CORNER_2:

            counter =
                "2 / 4";

            break;

        case CALIBRATION_CORNER_3:

            counter =
                "3 / 4";

            break;

        case CALIBRATION_CORNER_4:

            counter =
                "4 / 4";

            break;

        default:

            break;
    }

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
// ECRAN ORIENTATION
// ============================================================
//
// IMPORTANT :
// Aucun TFT_eSprite ici.
//
// Le problème venait de pushRotated().
// Cette fonction attend un TFT_eSprite comme destination,
// pas un TFT_eSPI.
//
// On dessine donc directement le "OK" retourné.
// ============================================================

static void drawOrientationScreen()
{
    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    // ========================================================
    // CADRE
    // ========================================================

    tft.drawRect(
        2,
        2,
        SCREEN_WIDTH - 4,
        SCREEN_HEIGHT - 4,
        COLOR_WHITE
    );

    // ========================================================
    // TITRE
    // ========================================================

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
        62,
        COLOR_CYAN,
        1
    );

    // ========================================================
    // ROTATION 0
    // ========================================================

    tft.drawRect(
        20,
        90,
        SCREEN_WIDTH - 40,
        80,
        COLOR_GREEN
    );

    drawCenteredText(
        "OK",
        SCREEN_WIDTH / 2,
        125,
        COLOR_GREEN,
        4
    );

    drawCenteredText(
        "ROTATION 0",
        SCREEN_WIDTH / 2,
        155,
        COLOR_WHITE,
        1
    );

    // ========================================================
    // SEPARATION
    // ========================================================

    tft.drawLine(
        15,
        190,
        SCREEN_WIDTH - 15,
        190,
        COLOR_GREY
    );

    // ========================================================
    // ROTATION 2
    // ========================================================

    tft.drawRect(
        20,
        215,
        SCREEN_WIDTH - 40,
        90,
        COLOR_YELLOW
    );

    // ========================================================
    // "OK" RETOURNE
    //
    // Dessin manuel.
    //
    // Pas de TFT_eSprite.
    // Pas de pushRotated().
    // ========================================================

    const int16_t cx =
        SCREEN_WIDTH / 2;

    const int16_t cy =
        252;

    const uint16_t color =
        COLOR_YELLOW;

    // --------------------------------------------------------
    // O RETOURNE
    //
    // Un O est symetrique, donc sa forme ne change pas.
    // --------------------------------------------------------

    tft.drawCircle(
        cx - 18,
        cy,
        13,
        color
    );

    // --------------------------------------------------------
    // K RETOURNE
    //
    // Axe vertical
    // --------------------------------------------------------

    const int16_t kx =
        cx + 8;

    tft.drawLine(
        kx + 12,
        cy - 13,
        kx - 3,
        cy,
        color
    );

    tft.drawLine(
        kx - 3,
        cy,
        kx + 12,
        cy + 13,
        color
    );

    tft.drawLine(
        kx - 3,
        cy - 13,
        kx - 3,
        cy + 13,
        color
    );

    drawCenteredText(
        "ROTATION 2",
        SCREEN_WIDTH / 2,
        285,
        COLOR_WHITE,
        1
    );

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

    for (
        uint8_t i = 0;
        i < 4;
        i++
    )
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

    // ========================================================
    // MOYENNES
    // ========================================================

    int32_t x0 =
        (
            (int32_t)rawX[0] +
            (int32_t)rawX[1]
        ) / 2;

    int32_t x1 =
        (
            (int32_t)rawX[3] +
            (int32_t)rawX[2]
        ) / 2;

    int32_t y0 =
        (
            (int32_t)rawY[0] +
            (int32_t)rawY[3]
        ) / 2;

    int32_t y1 =
        (
            (int32_t)rawY[1] +
            (int32_t)rawY[2]
        ) / 2;

    if (
        x0 > x1
    )
    {
        int32_t temp =
            x0;

        x0 =
            x1;

        x1 =
            temp;
    }

    if (
        y0 > y1
    )
    {
        int32_t temp =
            y0;

        y0 =
            y1;

        y1 =
            temp;
    }

    int32_t dx =
        x1 - x0;

    int32_t dy =
        y1 - y0;

    if (
        dx <= 0 ||
        dy <= 0
    )
    {
        Serial.println(
            "[TOUCH] ERREUR calibration invalide"
        );

        return false;
    }

    // ========================================================
    // TFT_eSPI
    // ========================================================

    calData[0] =
        (uint16_t)x0;

    calData[1] =
        (uint16_t)dx;

    calData[2] =
        (uint16_t)y0;

    calData[3] =
        (uint16_t)dy;

    calData[4] =
        0;

    tft.setTouch(
        calData
    );

    Serial.println(
        "[TOUCH] Parametres appliques"
    );

    Serial.print(
        "  X0 = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "  DX = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "  Y0 = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "  DY = "
    );

    Serial.println(
        calData[3]
    );

    return true;
}

// ============================================================
// INITIALISATION
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
    // --------------------------------------------------------
    // ETAT INITIAL
    // --------------------------------------------------------

    calibrationState =
        CALIBRATION_WAIT_RELEASE;

    selectedRotation =
        SCREEN_ROTATION;

    touchPressed =
        false;

    stableCount =
        0;

    stableX =
        0;

    stableY =
        0;

    lastTouchTime =
        0;

    // --------------------------------------------------------
    // RESET RAW
    // --------------------------------------------------------

    for (
        uint8_t i = 0;
        i < 4;
        i++
    )
    {
        rawX[i] =
            0;

        rawY[i] =
            0;
    }

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        calData[i] =
            0;
    }

    // --------------------------------------------------------
    // AFFICHAGE
    // --------------------------------------------------------

    drawCalibrationScreen();

    // --------------------------------------------------------
    // STABILISATION
    // --------------------------------------------------------

    delay(
        CALIBRATION_START_DELAY
    );

    calibrationStartTime =
        millis();

    Serial.println();
    Serial.println(
        "[TOUCH] Calibration tactile"
    );

    Serial.println(
        "[TOUCH] Attente relachement..."
    );
}

// ============================================================
// LECTURE STABLE
// ============================================================

static bool readStableTouch(
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

    // --------------------------------------------------------
    // PAS DE TOUCHER
    // --------------------------------------------------------

    if (
        !pressed
    )
    {
        stableCount =
            0;

        return false;
    }

    // --------------------------------------------------------
    // PREMIERE LECTURE
    // --------------------------------------------------------

    if (
        stableCount == 0
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
    // STABILITE
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
    // VALIDATION
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

        return true;
    }

    return false;
}

// ============================================================
// UPDATE
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    // ========================================================
    // ATTENTE RELACHEMENT
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_WAIT_RELEASE
    )
    {
        return false;
    }

    // ========================================================
    // COIN 1
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_CORNER_1
    )
    {
        rawX[0] =
            x;

        rawY[0] =
            y;

        Serial.println(
            "[TOUCH] Coin 1 valide"
        );

        Serial.print(
            "RAW X="
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

        calibrationState =
            CALIBRATION_CORNER_2;

        drawCalibrationScreen();

        return true;
    }

    // ========================================================
    // COIN 2
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_CORNER_2
    )
    {
        rawX[1] =
            x;

        rawY[1] =
            y;

        Serial.println(
            "[TOUCH] Coin 2 valide"
        );

        calibrationState =
            CALIBRATION_CORNER_3;

        drawCalibrationScreen();

        return true;
    }

    // ========================================================
    // COIN 3
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_CORNER_3
    )
    {
        rawX[2] =
            x;

        rawY[2] =
            y;

        Serial.println(
            "[TOUCH] Coin 3 valide"
        );

        calibrationState =
            CALIBRATION_CORNER_4;

        drawCalibrationScreen();

        return true;
    }

    // ========================================================
    // COIN 4
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_CORNER_4
    )
    {
        rawX[3] =
            x;

        rawY[3] =
            y;

        Serial.println(
            "[TOUCH] Coin 4 valide"
        );

        if (
            !calculateTouchCalibration()
        )
        {
            calibrationState =
                CALIBRATION_CORNER_1;

            drawCalibrationScreen();

            return false;
        }

        calibrationState =
            CALIBRATION_ORIENTATION;

        drawOrientationScreen();

        Serial.println();
        Serial.println(
            "[TOUCH] 4 coins termines"
        );

        Serial.println(
            "[TOUCH] Choisir rotation 0 ou 2"
        );

        return true;
    }

    // ========================================================
    // ORIENTATION
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_ORIENTATION
    )
    {
        // ----------------------------------------------------
        // ROTATION 0
        // ----------------------------------------------------

        if (
            y >= 90 &&
            y < 170
        )
        {
            selectedRotation =
                0;
        }

        // ----------------------------------------------------
        // ROTATION 2
        // ----------------------------------------------------

        else if (
            y >= 215 &&
            y < 305
        )
        {
            selectedRotation =
                2;
        }

        else
        {
            return false;
        }

        Serial.print(
            "[TOUCH] Rotation choisie = "
        );

        Serial.println(
            selectedRotation
        );

        // ----------------------------------------------------
        // APPLICATION
        // ----------------------------------------------------

        tft.setTouch(
            calData
        );

        tft.setRotation(
            selectedRotation
        );

        // ----------------------------------------------------
        // EEPROM
        // ----------------------------------------------------

        if (
            saveTouchCalibration()
        )
        {
            Serial.println(
                "[TOUCH] EEPROM : sauvegarde OK"
            );
        }
        else
        {
            Serial.println(
                "[TOUCH] EEPROM : erreur sauvegarde"
            );
        }

        // ----------------------------------------------------
        // FIN
        // ----------------------------------------------------

        calibrationState =
            CALIBRATION_DONE;

        drawCalibrationFinishedScreen();

        return true;
    }

    return false;
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
        calculateChecksum(
            data
        )
    )
    {
        return false;
    }

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
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

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
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