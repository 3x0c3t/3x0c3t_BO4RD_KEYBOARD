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
// TFT CALIBRATION DATA
//
// TFT_eSPI attend :
//
// calData[0] = X minimum
// calData[1] = X maximum
// calData[2] = Y minimum
// calData[3] = Y maximum
// calData[4] = inversion / swap
//
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
// STABILISATION
// ============================================================

static uint16_t stableX = 0;
static uint16_t stableY = 0;

static uint8_t stableCount = 0;

static bool calibrationTouchLocked =
    true;

// ============================================================
// PARAMETRES
// ============================================================

static const uint32_t CALIBRATION_START_DELAY =
    1200;

static const uint32_t RELEASE_TIMEOUT =
    5000;

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
        checksum +=
            data.calData[i];
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
    const int16_t size = 10;

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
    // --------------------------------------------------------
    // Rotation fixe pendant toute la calibration
    // --------------------------------------------------------

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
    // COINS
    //
    // IMPORTANT :
    // Les coins sont dessinés AVANT le texte.
    // --------------------------------------------------------

    const int16_t margin = 8;

    const int16_t left =
        margin;

    const int16_t right =
        SCREEN_WIDTH - 1 - margin;

    const int16_t top =
        margin;

    const int16_t bottom =
        SCREEN_HEIGHT - 1 - margin;

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

    // --------------------------------------------------------
    // TITRE
    // --------------------------------------------------------

    drawCenteredText(
        "3x0c3t_BO4RD",
        SCREEN_WIDTH / 2,
        105,
        COLOR_WHITE,
        2
    );

    drawCenteredText(
        "Calibration Affichage",
        SCREEN_WIDTH / 2,
        135,
        COLOR_CYAN,
        1
    );

    drawCenteredText(
        "& Tactile",
        SCREEN_WIDTH / 2,
        153,
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

    switch (
        calibrationState
    )
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

        case CALIBRATION_WAIT_RELEASE:

            instruction =
                "Relacher l'ecran";

            counter =
                "ATTENTE";

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
        235,
        COLOR_GREEN,
        2
    );

    tft.setTextDatum(
        TL_DATUM
    );
}

// ============================================================
// TEXTE OK ROTATION 180
// ============================================================

static void drawUpsideDownOK(
    int16_t centerX,
    int16_t centerY
)
{
    TFT_eSprite sprite =
        TFT_eSprite(
            &tft
        );

    if (
        !sprite.createSprite(
            70,
            50
        )
    )
    {
        return;
    }

    sprite.fillSprite(
        COLOR_BLACK
    );

    sprite.setTextDatum(
        MC_DATUM
    );

    sprite.setTextColor(
        COLOR_YELLOW,
        COLOR_BLACK
    );

    sprite.setTextSize(
        4
    );

    sprite.drawString(
        "OK",
        35,
        25
    );

    // --------------------------------------------------------
    // Rotation 180
    // --------------------------------------------------------

    sprite.pushRotated(
        &sprite,
        180,
        COLOR_BLACK
    );

    sprite.deleteSprite();
}

// ============================================================
// ECRAN ORIENTATION
// ============================================================

static void drawOrientationScreen()
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
        62,
        COLOR_CYAN,
        1
    );

    // --------------------------------------------------------
    // ZONE OK NORMAL
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // SEPARATION
    // --------------------------------------------------------

    tft.drawLine(
        15,
        190,
        SCREEN_WIDTH - 15,
        190,
        COLOR_GREY
    );

    // --------------------------------------------------------
    // ZONE OK RETOURNE
    // --------------------------------------------------------

    tft.drawRect(
        20,
        215,
        SCREEN_WIDTH - 40,
        90,
        COLOR_YELLOW
    );

    // --------------------------------------------------------
    // Texte retourné
    // --------------------------------------------------------

    TFT_eSprite sprite =
        TFT_eSprite(
            &tft
        );

    if (
        sprite.createSprite(
            70,
            50
        )
    )
    {
        sprite.fillSprite(
            COLOR_BLACK
        );

        sprite.setTextDatum(
            MC_DATUM
        );

        sprite.setTextColor(
            COLOR_YELLOW,
            COLOR_BLACK
        );

        sprite.setTextSize(
            4
        );

        sprite.drawString(
            "OK",
            35,
            25
        );

        // ----------------------------------------------------
        // Rotation 180
        // ----------------------------------------------------

        sprite.pushRotated(
            &tft,
            180,
            COLOR_BLACK
        );

        sprite.deleteSprite();
    }

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

    // --------------------------------------------------------
    // X
    //
    // Coin 1 = haut gauche
    // Coin 2 = haut droit
    // Coin 3 = bas droit
    // Coin 4 = bas gauche
    // --------------------------------------------------------

    uint16_t xMin =
        min(
            rawX[0],
            rawX[3]
        );

    uint16_t xMax =
        max(
            rawX[1],
            rawX[2]
        );

    // --------------------------------------------------------
    // Y
    // --------------------------------------------------------

    uint16_t yMin =
        min(
            rawY[0],
            rawY[1]
        );

    uint16_t yMax =
        max(
            rawY[2],
            rawY[3]
        );

    // --------------------------------------------------------
    // Vérification
    // --------------------------------------------------------

    if (
        xMax <= xMin ||
        yMax <= yMin
    )
    {
        Serial.println(
            "[TOUCH] ERREUR : calibration invalide"
        );

        Serial.print(
            "Xmin="
        );

        Serial.print(
            xMin
        );

        Serial.print(
            " Xmax="
        );

        Serial.println(
            xMax
        );

        Serial.print(
            "Ymin="
        );

        Serial.print(
            yMin
        );

        Serial.print(
            " Ymax="
        );

        Serial.println(
            yMax
        );

        return false;
    }

    // --------------------------------------------------------
    // TFT_eSPI
    // --------------------------------------------------------

    calData[0] =
        xMin;

    calData[1] =
        xMax;

    calData[2] =
        yMin;

    calData[3] =
        yMax;

    calData[4] =
        0;

    tft.setTouch(
        calData
    );

    Serial.println(
        "[TOUCH] Parametres appliques"
    );

    Serial.print(
        "  X MIN = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "  X MAX = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "  Y MIN = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "  Y MAX = "
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
// DEMARRAGE
// ============================================================

void startTouchCalibration()
{
    // --------------------------------------------------------
    // RESET COMPLET
    // --------------------------------------------------------

    calibrationState =
        CALIBRATION_WAIT_RELEASE;

    selectedRotation =
        SCREEN_ROTATION;

    stableCount =
        0;

    stableX =
        0;

    stableY =
        0;

    calibrationTouchLocked =
        true;

    for (
        uint8_t i = 0;
        i < 4;
        i++
    )
    {
        rawX[i] = 0;
        rawY[i] = 0;
    }

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        calData[i] = 0;
    }

    // --------------------------------------------------------
    // AFFICHER D'ABORD L'ECRAN
    // --------------------------------------------------------

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    drawCalibrationScreen();

    // --------------------------------------------------------
    // LAISSER LE TFT SE STABILISER
    // --------------------------------------------------------

    delay(
        CALIBRATION_START_DELAY
    );

    Serial.println();
    Serial.println(
        "[TOUCH] Calibration tactile"
    );

    Serial.println(
        "[TOUCH] Purge du toucher..."
    );

    // --------------------------------------------------------
    // PURGE DU TOUCHER
    //
    // On ne veut surtout pas qu'une pression présente
    // pendant le démarrage devienne le point 1.
    // --------------------------------------------------------

    uint32_t start =
        millis();

    uint16_t dummyX = 0;
    uint16_t dummyY = 0;

    while (
        millis() - start <
        RELEASE_TIMEOUT
    )
    {
        if (
            !tft.getTouchRaw(
                &dummyX,
                &dummyY
            )
        )
        {
            break;
        }

        delay(
            20
        );

        yield();
    }

    // --------------------------------------------------------
    // VERROUILLAGE TERMINE
    // --------------------------------------------------------

    calibrationTouchLocked =
        false;

    stableCount =
        0;

    calibrationState =
        CALIBRATION_CORNER_1;

    drawCalibrationScreen();

    Serial.println(
        "[TOUCH] Ecran pret"
    );

    Serial.println(
        "[TOUCH] Attente toucher coin 1"
    );
}

// ============================================================
// UPDATE
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    // --------------------------------------------------------
    // VERROUILLAGE
    // --------------------------------------------------------

    if (
        calibrationTouchLocked
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // ETAT COIN 1
    // --------------------------------------------------------

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

        stableCount =
            0;

        drawCalibrationScreen();

        return true;
    }

    // --------------------------------------------------------
    // COIN 2
    // --------------------------------------------------------

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
            CALIBRATION_CORNER_3;

        stableCount =
            0;

        drawCalibrationScreen();

        return true;
    }

    // --------------------------------------------------------
    // COIN 3
    // --------------------------------------------------------

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
            CALIBRATION_CORNER_4;

        stableCount =
            0;

        drawCalibrationScreen();

        return true;
    }

    // --------------------------------------------------------
    // COIN 4
    // --------------------------------------------------------

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

        // ----------------------------------------------------
        // CALCUL
        // ----------------------------------------------------

        if (
            !calculateTouchCalibration()
        )
        {
            Serial.println(
                "[TOUCH] Calibration invalide"
            );

            calibrationState =
                CALIBRATION_CORNER_1;

            stableCount =
                0;

            drawCalibrationScreen();

            return false;
        }

        // ----------------------------------------------------
        // ORIENTATION
        // ----------------------------------------------------

        calibrationState =
            CALIBRATION_ORIENTATION;

        stableCount =
            0;

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

    // --------------------------------------------------------
    // ORIENTATION
    // --------------------------------------------------------

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
        // APPLIQUER
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

    // --------------------------------------------------------
    // MAGIC
    // --------------------------------------------------------

    if (
        data.magic !=
        TOUCH_EEPROM_MAGIC
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // VERSION
    // --------------------------------------------------------

    if (
        data.version !=
        TOUCH_EEPROM_VERSION
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // RESOLUTION
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // ROTATION
    // --------------------------------------------------------

    if (
        data.rotation != 0
        &&
        data.rotation != 2
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // CHECKSUM
    // --------------------------------------------------------

    if (
        data.checksum !=
        calculateChecksum(
            data
        )
    )
    {
        return false;
    }

    // --------------------------------------------------------
    // COPIE
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // APPLICATION
    // --------------------------------------------------------

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