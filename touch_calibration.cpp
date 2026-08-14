#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// DONNEES EEPROM
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
// CALIBRATION EN RAM
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
// TEXTE OK - ORIENTATION 0
// ============================================================

static void drawOK0(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
    tft.setTextDatum(
        MC_DATUM
    );

    tft.setTextColor(
        fg,
        bg
    );

    tft.setTextSize(
        3
    );

    tft.drawString(
        "OK",
        x,
        y
    );
}

// ============================================================
// TEXTE OK - ORIENTATION 90
// ============================================================

static void drawOK90(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
    uint16_t oldRotation =
        SCREEN_ROTATION;

    tft.setRotation(
        1
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.setTextColor(
        fg,
        bg
    );

    tft.setTextSize(
        3
    );

    tft.drawString(
        "OK",
        y,
        SCREEN_WIDTH - x
    );

    tft.setRotation(
        oldRotation
    );
}

// ============================================================
// TEXTE OK - ORIENTATION 180
// ============================================================

static void drawOK180(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
    uint16_t oldRotation =
        SCREEN_ROTATION;

    tft.setRotation(
        2
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.setTextColor(
        fg,
        bg
    );

    tft.setTextSize(
        3
    );

    tft.drawString(
        "OK",
        SCREEN_WIDTH - x,
        SCREEN_HEIGHT - y
    );

    tft.setRotation(
        oldRotation
    );
}

// ============================================================
// TEXTE OK - ORIENTATION 270
// ============================================================

static void drawOK270(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
    uint16_t oldRotation =
        SCREEN_ROTATION;

    tft.setRotation(
        3
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.setTextColor(
        fg,
        bg
    );

    tft.setTextSize(
        3
    );

    tft.drawString(
        "OK",
        SCREEN_HEIGHT - y,
        x
    );

    tft.setRotation(
        oldRotation
    );
}

// ============================================================
// ECRAN D'ORIENTATION
// ============================================================
//
//             0                     239
//
//              HAUT
//
//        +-------------------------+
//        |            |            |
//        |    OK      |      OK    |
//        |    0°      |      90°   |
//        |       \     |     /     |
//        |        \    |    /      |
//        |         \   |   /       |
//        |          \  |  /        |
//        |           \ | /         |
//        |            \●/          |
//        |             X           |
//        |            / \          |
//        |           /   \         |
//        |          /     \        |
//        |         /       \       |
//        |        /         \      |
//        |   OK  /           \ OK  |
//        | 270° /             \180°|
//        +-------------------------+
//
// ============================================================

static void drawTriangleOrientationScreen()
{
    const int16_t cx =
        SCREEN_WIDTH / 2;

    const int16_t cy =
        SCREEN_HEIGHT / 2;

    const int16_t w =
        SCREEN_WIDTH - 1;

    const int16_t h =
        SCREEN_HEIGHT - 1;

    // ========================================================
    // ROTATION NORMALE
    // ========================================================

    tft.setRotation(
        SCREEN_ROTATION
    );

    // ========================================================
    // FOND
    // ========================================================

    tft.fillScreen(
        COLOR_BLACK
    );

    // ========================================================
    // TRIANGLE HAUT-GAUCHE
    // ========================================================

    tft.fillTriangle(
        0,
        0,
        w,
        0,
        cx,
        cy,
        COLOR_RED
    );

    // ========================================================
    // TRIANGLE HAUT-DROITE
    // ========================================================

    tft.fillTriangle(
        w,
        0,
        w,
        h,
        cx,
        cy,
        COLOR_GREEN
    );

    // ========================================================
    // TRIANGLE BAS-DROITE
    // ========================================================

    tft.fillTriangle(
        w,
        h,
        0,
        h,
        cx,
        cy,
        COLOR_BLUE
    );

    // ========================================================
    // TRIANGLE BAS-GAUCHE
    // ========================================================

    tft.fillTriangle(
        0,
        h,
        0,
        0,
        cx,
        cy,
        COLOR_YELLOW
    );

    // ========================================================
    // SEPARATIONS
    // ========================================================

    tft.drawLine(
        0,
        0,
        cx,
        cy,
        COLOR_WHITE
    );

    tft.drawLine(
        w,
        0,
        cx,
        cy,
        COLOR_WHITE
    );

    tft.drawLine(
        w,
        h,
        cx,
        cy,
        COLOR_WHITE
    );

    tft.drawLine(
        0,
        h,
        cx,
        cy,
        COLOR_WHITE
    );

    // ========================================================
    // CENTRE
    // ========================================================

    tft.fillCircle(
        cx,
        cy,
        5,
        COLOR_WHITE
    );

    // ========================================================
    // POSITIONS DES OK
    // ========================================================

    const int16_t qx =
        SCREEN_WIDTH / 4;

    const int16_t qy =
        SCREEN_HEIGHT / 4;

    // ========================================================
    // HAUT-GAUCHE
    // ========================================================

    drawOK0(
        qx,
        qy,
        COLOR_WHITE,
        COLOR_RED
    );

    // ========================================================
    // HAUT-DROITE
    // ========================================================

    drawOK90(
        SCREEN_WIDTH * 3 / 4,
        SCREEN_HEIGHT / 4,
        COLOR_WHITE,
        COLOR_GREEN
    );

    // ========================================================
    // BAS-DROITE
    // ========================================================

    drawOK180(
        SCREEN_WIDTH * 3 / 4,
        SCREEN_HEIGHT * 3 / 4,
        COLOR_WHITE,
        COLOR_BLUE
    );

    // ========================================================
    // BAS-GAUCHE
    // ========================================================

    drawOK270(
        SCREEN_WIDTH / 4,
        SCREEN_HEIGHT * 3 / 4,
        COLOR_BLACK,
        COLOR_YELLOW
    );

    // ========================================================
    // RESTAURATION ROTATION
    // ========================================================

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.setTextDatum(
        TL_DATUM
    );

    tft.setTextSize(
        1
    );
}

// ============================================================
// EEPROM - CHARGEMENT
// ============================================================

bool loadTouchCalibration()
{
    TouchCalibrationData data;

    Serial.println(
        "[TOUCH EEPROM] Lecture..."
    );

    EEPROM.begin(
        TOUCH_EEPROM_SIZE
    );

    EEPROM.get(
        TOUCH_EEPROM_ADDRESS,
        data
    );

    EEPROM.end();

    // ========================================================
    // MAGIC
    // ========================================================

    if (
        data.magic != TOUCH_EEPROM_MAGIC
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Aucune calibration"
        );

        return false;
    }

    // ========================================================
    // VERSION
    // ========================================================

    if (
        data.version != TOUCH_EEPROM_VERSION
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Version incompatible"
        );

        return false;
    }

    // ========================================================
    // RESOLUTION
    // ========================================================

    if (
        data.width != SCREEN_WIDTH ||
        data.height != SCREEN_HEIGHT
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Resolution incompatible"
        );

        return false;
    }

    // ========================================================
    // ROTATION
    // ========================================================

    if (
        data.rotation != SCREEN_ROTATION
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Rotation incompatible"
        );

        return false;
    }

    // ========================================================
    // CHECKSUM
    // ========================================================

    uint16_t expectedChecksum =
        calculateChecksum(
            data
        );

    if (
        data.checksum != expectedChecksum
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Checksum invalide"
        );

        return false;
    }

    // ========================================================
    // COPIE
    // ========================================================

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        calData[i] =
            data.calData[i];
    }

    // ========================================================
    // ACTIVATION
    // ========================================================

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.setTouch(
        calData
    );

    // ========================================================
    // LOG
    // ========================================================

    Serial.println(
        "[TOUCH EEPROM] Calibration valide"
    );

    Serial.println(
        "[TOUCH EEPROM] Calibration chargee"
    );

    Serial.print(
        "[TOUCH EEPROM] CAL0 = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "[TOUCH EEPROM] CAL1 = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "[TOUCH EEPROM] CAL2 = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "[TOUCH EEPROM] CAL3 = "
    );

    Serial.println(
        calData[3]
    );

    Serial.print(
        "[TOUCH EEPROM] CAL4 = "
    );

    Serial.println(
        calData[4]
    );

    return true;
}

// ============================================================
// EEPROM - SAUVEGARDE
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
        SCREEN_ROTATION;

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

    Serial.println(
        "[TOUCH EEPROM] Sauvegarde..."
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

    if (
        !result
    )
    {
        Serial.println(
            "[TOUCH EEPROM] ERREUR commit()"
        );

        return false;
    }

    Serial.println(
        "[TOUCH EEPROM] Sauvegarde OK"
    );

    return true;
}

// ============================================================
// EEPROM - EFFACEMENT
// ============================================================

bool clearTouchCalibration()
{
    Serial.println(
        "[TOUCH EEPROM] Effacement..."
    );

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

    if (
        result
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Effacement OK"
        );
    }
    else
    {
        Serial.println(
            "[TOUCH EEPROM] Erreur effacement"
        );
    }

    return result;
}

// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration()
{
    Serial.println();
    Serial.println(
        "[TOUCH] ================================"
    );

    Serial.println(
        "[TOUCH] CALIBRATION"
    );

    Serial.print(
        "[TOUCH] Rotation = "
    );

    Serial.println(
        SCREEN_ROTATION
    );

    Serial.print(
        "[TOUCH] Ecran = "
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
        "[TOUCH] Affichage orientation"
    );

    // ========================================================
    // ECRAN D'ORIENTATION
    // ========================================================

    drawTriangleOrientationScreen();

    delay(
        2000
    );

    Serial.println(
        "[TOUCH] Orientation affichee"
    );

    Serial.println(
        "[TOUCH] Lancement calibration tactile"
    );

    delay(
        500
    );

    // ========================================================
    // CALIBRATION TFT_eSPI
    // ========================================================

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    // ========================================================
    // ACTIVATION
    // ========================================================

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.setTouch(
        calData
    );

    // ========================================================
    // RESULTATS
    // ========================================================

    Serial.println();
    Serial.println(
        "[TOUCH] Calibration terminee"
    );

    Serial.print(
        "[TOUCH] CAL0 = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "[TOUCH] CAL1 = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "[TOUCH] CAL2 = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "[TOUCH] CAL3 = "
    );

    Serial.println(
        calData[3]
    );

    Serial.print(
        "[TOUCH] CAL4 = "
    );

    Serial.println(
        calData[4]
    );

    // ========================================================
    // EEPROM
    // ========================================================

    if (
        saveTouchCalibration()
    )
    {
        Serial.println(
            "[TOUCH] Calibration sauvegardee en EEPROM"
        );
    }
    else
    {
        Serial.println(
            "[TOUCH] ERREUR sauvegarde EEPROM"
        );
    }

    Serial.println(
        "[TOUCH] Calibration active"
    );

    Serial.println();
}

// ============================================================
// COMPATIBILITE
// ============================================================

bool touchCalibrationChoice()
{
    return true;
}

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
)
{
    (void)x;
    (void)y;

    return true;
}

bool loadPreviousCalibration()
{
    return loadTouchCalibration();
}

bool touchCalibrationActive()
{
    return false;
}

bool touchCalibrationFinished()
{
    return true;
}