#include "touch_calibration.h"
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>

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
// ECRITURE TEXTE CENTREE
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
// TEXTE ROTATION 0
// ============================================================

static void drawOK_0(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
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
        x,
        y
    );
}

// ============================================================
// TEXTE ROTATION 90
// ============================================================

static void drawOK_90(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
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
        SCREEN_ROTATION
    );
}

// ============================================================
// TEXTE ROTATION 180
// ============================================================

static void drawOK_180(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
    tft.setRotation(
        0
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
        SCREEN_ROTATION
    );
}

// ============================================================
// TEXTE ROTATION 270
// ============================================================

static void drawOK_270(
    int16_t x,
    int16_t y,
    uint16_t fg,
    uint16_t bg
)
{
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
        SCREEN_ROTATION
    );
}

// ============================================================
// ECRAN CHOIX DISPOSITION
// ============================================================

static void drawDisplayOrientationChoice()
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
    // CADRE PRINCIPAL
    // ========================================================

    tft.drawRect(
        4,
        4,
        SCREEN_WIDTH - 8,
        SCREEN_HEIGHT - 8,
        COLOR_WHITE
    );

    tft.drawRect(
        7,
        7,
        SCREEN_WIDTH - 14,
        SCREEN_HEIGHT - 14,
        COLOR_GREY
    );

    // ========================================================
    // TITRE
    // ========================================================

    drawCenteredText(
        "3x0c3t_BO4RD",
        cx,
        28,
        COLOR_WHITE,
        2
    );

    // ========================================================
    // SOUS-TITRE
    // ========================================================

    drawCenteredText(
        "Calibration Affichage & Tactile",
        cx,
        55,
        COLOR_CYAN,
        1
    );

    // ========================================================
    // INSTRUCTION
    // ========================================================

    drawCenteredText(
        "Choisir la disposition de l'ecran...",
        cx,
        75,
        COLOR_WHITE,
        1
    );

    // ========================================================
    // ZONE DE CHOIX
    // ========================================================

    const int16_t topY =
        125;

    const int16_t bottomY =
        255;

    const int16_t leftX =
        45;

    const int16_t rightX =
        195;

    // ========================================================
    // TRIANGLES
    // ========================================================

    tft.fillTriangle(
        cx,
        cy,
        leftX,
        topY,
        rightX,
        topY,
        COLOR_RED
    );

    tft.fillTriangle(
        cx,
        cy,
        rightX,
        topY,
        rightX,
        bottomY,
        COLOR_GREEN
    );

    tft.fillTriangle(
        cx,
        cy,
        rightX,
        bottomY,
        leftX,
        bottomY,
        COLOR_BLUE
    );

    tft.fillTriangle(
        cx,
        cy,
        leftX,
        bottomY,
        leftX,
        topY,
        COLOR_YELLOW
    );

    // ========================================================
    // SEPARATIONS
    // ========================================================

    tft.drawLine(
        cx,
        cy,
        leftX,
        topY,
        COLOR_WHITE
    );

    tft.drawLine(
        cx,
        cy,
        rightX,
        topY,
        COLOR_WHITE
    );

    tft.drawLine(
        cx,
        cy,
        rightX,
        bottomY,
        COLOR_WHITE
    );

    tft.drawLine(
        cx,
        cy,
        leftX,
        bottomY,
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
    // OK HAUT
    // ========================================================

    drawOK_0(
        cx,
        105,
        COLOR_WHITE,
        COLOR_RED
    );

    // ========================================================
    // OK DROITE
    // ========================================================

    drawOK_90(
        175,
        cy,
        COLOR_WHITE,
        COLOR_GREEN
    );

    // ========================================================
    // OK BAS
    // ========================================================

    drawOK_180(
        cx,
        275,
        COLOR_WHITE,
        COLOR_BLUE
    );

    // ========================================================
    // OK GAUCHE
    // ========================================================

    drawOK_270(
        65,
        cy,
        COLOR_BLACK,
        COLOR_YELLOW
    );

    // ========================================================
    // RESTAURATION
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
// CALIBRATION
// ============================================================

void startTouchCalibration()
{
    Serial.println();
    Serial.println(
        "[TOUCH] ================================"
    );

    Serial.println(
        "[TOUCH] CALIBRATION AFFICHAGE & TACTILE"
    );

    Serial.println(
        "[TOUCH] Choix disposition"
    );

    Serial.println(
        "[TOUCH] ================================"
    );

    // ========================================================
    // AFFICHAGE
    // ========================================================

    drawDisplayOrientationChoice();

    // ========================================================
    // ATTENTE CHOIX
    // ========================================================

    Serial.println(
        "[TOUCH] Attente choix disposition..."
    );

    // Pour l'instant on reste dans cet écran.
    // La détection des quatre triangles sera ajoutée ici.
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

    if (
        data.magic != TOUCH_EEPROM_MAGIC
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Aucune calibration"
        );

        return false;
    }

    if (
        data.version != TOUCH_EEPROM_VERSION
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Version incompatible"
        );

        return false;
    }

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

    if (
        data.rotation != SCREEN_ROTATION
    )
    {
        Serial.println(
            "[TOUCH EEPROM] Rotation incompatible"
        );

        return false;
    }

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

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        calData[i] =
            data.calData[i];
    }

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.setTouch(
        calData
    );

    Serial.println(
        "[TOUCH EEPROM] Calibration chargee"
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
// EEPROM - EFFACEMENT
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