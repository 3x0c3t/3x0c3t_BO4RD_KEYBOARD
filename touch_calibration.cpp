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
// STRUCTURE EEPROM
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
// CHARGEMENT EEPROM
// ============================================================

bool loadTouchCalibration()
{
    TouchCalibrationData data;

    Serial.println(
        "[TOUCH EEPROM] Lecture..."
    );

    // ESP8266 EEPROM.begin() retourne VOID
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
    // COPIE EN RAM
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
// SAUVEGARDE EEPROM
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

    // ESP8266 EEPROM.begin() retourne VOID
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
// EFFACEMENT EEPROM
// ============================================================

bool clearTouchCalibration()
{
    Serial.println(
        "[TOUCH EEPROM] Effacement..."
    );

    // ESP8266 EEPROM.begin() retourne VOID
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
        "[TOUCH] DEBUT CALIBRATION"
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
        "[TOUCH] Touchez les points affiches"
    );

    Serial.println(
        "[TOUCH] ================================"
    );

    // ========================================================
    // CALIBRATION TFT_eSPI
    // ========================================================

    tft.calibrateTouch(
        calData,
        COLOR_MAGENTA,
        COLOR_BLACK,
        15
    );

    // ========================================================
    // ACTIVATION
    // ========================================================

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
    // SAUVEGARDE EEPROM
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