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
//
// IMPORTANT
//
// TFT_eSPI attend :
//
// [0] = X0
// [1] = X1 - X0
// [2] = Y0
// [3] = Y1 - Y0
// [4] = flags
//
// flags :
//
// bit 0 = axes X/Y inverses
// bit 1 = inversion X
// bit 2 = inversion Y
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
// ORIENTATION TFT
// ============================================================

static uint8_t selectedRotation =
    SCREEN_ROTATION;

// ============================================================
// STABILISATION
// ============================================================

static uint16_t stableX = 0;
static uint16_t stableY = 0;

static uint8_t stableCount = 0;

static bool touchWasDetected = false;

static uint32_t lastTouchTime = 0;

// ============================================================
// PARAMETRES
// ============================================================

// Temps avant de commencer la calibration
static const uint32_t CALIBRATION_START_DELAY =
    1200;

// Debounce entre deux validations
static const uint32_t TOUCH_DEBOUNCE_TIME =
    400;

// Nombre de mesures stables
static const uint8_t REQUIRED_STABLE_SAMPLES =
    4;

// Tolérance RAW
static const uint16_t STABLE_TOLERANCE =
    100;

// Seuil de pression.
// TFT_eSPI utilise normalement 350.
// Pour les coins, Bodmer utilise 350 / 2.
// On utilise donc 175 pour faciliter les coins.
static const uint16_t CALIBRATION_Z_THRESHOLD =
    175;

// Nombre d'échantillons utilisés pour une calibration
static const uint8_t CALIBRATION_SAMPLES =
    8;

// Marge graphique des cibles
static const int16_t CORNER_MARGIN =
    18;

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

    tft.setTextDatum(
        TL_DATUM
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
    const int16_t size = 16;

    // Croix
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

    // Cercle extérieur
    tft.drawCircle(
        x,
        y,
        10,
        COLOR_RED
    );

    // Centre
    tft.fillCircle(
        x,
        y,
        3,
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
    // CIBLES
    // --------------------------------------------------------

    const int16_t margin =
        CORNER_MARGIN;

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
    // CADRE CENTRAL
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

    // --------------------------------------------------------
    // TITRE
    // --------------------------------------------------------

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
    // INSTRUCTIONS
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
}

// ============================================================
// BOUTON ORIENTATION 0
// ============================================================

static void drawOrientation0()
{
    tft.fillRect(
        15,
        85,
        SCREEN_WIDTH - 30,
        95,
        COLOR_BLACK
    );

    tft.drawRect(
        15,
        85,
        SCREEN_WIDTH - 30,
        95,
        COLOR_GREEN
    );

    drawCenteredText(
        "OK",
        SCREEN_WIDTH / 2,
        122,
        COLOR_GREEN,
        4
    );

    drawCenteredText(
        "ORIENTATION 0",
        SCREEN_WIDTH / 2,
        158,
        COLOR_WHITE,
        1
    );
}

// ============================================================
// BOUTON ORIENTATION 2
// ============================================================

static void drawOrientation2()
{
    tft.fillRect(
        15,
        200,
        SCREEN_WIDTH - 30,
        105,
        COLOR_BLACK
    );

    tft.drawRect(
        15,
        200,
        SCREEN_WIDTH - 30,
        105,
        COLOR_YELLOW
    );

    const int16_t cx =
        SCREEN_WIDTH / 2;

    const int16_t cy =
        250;

    // --------------------------------------------------------
    // O
    // --------------------------------------------------------

    tft.drawCircle(
        cx + 18,
        cy,
        14,
        COLOR_YELLOW
    );

    // --------------------------------------------------------
    // K
    // --------------------------------------------------------

    tft.drawLine(
        cx - 5,
        cy - 14,
        cx - 5,
        cy + 14,
        COLOR_YELLOW
    );

    tft.drawLine(
        cx - 5,
        cy,
        cx - 20,
        cy - 14,
        COLOR_YELLOW
    );

    tft.drawLine(
        cx - 5,
        cy,
        cx - 20,
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
        32,
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
    // ORIENTATION 0
    // --------------------------------------------------------

    drawOrientation0();

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
    // ORIENTATION 2
    // --------------------------------------------------------

    drawOrientation2();
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

    delay(800);
}

// ============================================================
// ATTENDRE RELACHEMENT RAW
// ============================================================

static void waitRawTouchRelease()
{
    uint32_t start =
        millis();

    while (
        millis() - start <
        2000
    )
    {
        uint16_t z =
            tft.getTouchRawZ();

        if (
            z < CALIBRATION_Z_THRESHOLD
        )
        {
            delay(100);
            return;
        }

        yield();
        delay(10);
    }

    Serial.println(
        "[TOUCH] Timeout relachement"
    );
}

// ============================================================
// LECTURE RAW STABLE
//
// IMPORTANT :
//
// getTouchRaw() retourne toujours true.
//
// On utilise donc getTouchRawZ()
// pour savoir si l'ecran est réellement
// pressé.
// ============================================================

static bool readStableRawTouch(
    uint16_t& x,
    uint16_t& y
)
{
    uint16_t currentX = 0;
    uint16_t currentY = 0;

    // --------------------------------------------------------
    // PRESSION
    // --------------------------------------------------------

    uint16_t z =
        tft.getTouchRawZ();

    if (
        z < CALIBRATION_Z_THRESHOLD
    )
    {
        stableCount = 0;
        touchWasDetected = false;

        return false;
    }

    // --------------------------------------------------------
    // PREMIERE LECTURE
    // --------------------------------------------------------

    tft.getTouchRaw(
        &currentX,
        &currentY
    );

    if (
        stableCount == 0
    )
    {
        stableX =
            currentX;

        stableY =
            currentY;

        stableCount = 1;

        return false;
    }

    // --------------------------------------------------------
    // VERIFICATION STABILITE
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

        stableCount = 1;

        return false;
    }

    // --------------------------------------------------------
    // TOUCH CONFIRME
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

        stableCount = 0;

        touchWasDetected = true;

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

        Serial.print(
            y
        );

        Serial.print(
            " Z="
        );

        Serial.println(
            z
        );

        return true;
    }

    return false;
}

// ============================================================
// CAPTURE D'UN COIN
//
// Plusieurs mesures sont prises et moyennées.
// Cela évite qu'une seule lecture parasite
// détruise la calibration.
// ============================================================

static bool captureCalibrationPoint(
    uint16_t& resultX,
    uint16_t& resultY
)
{
    uint32_t sumX = 0;
    uint32_t sumY = 0;

    uint8_t samples = 0;

    uint32_t start =
        millis();

    while (
        samples <
        CALIBRATION_SAMPLES
    )
    {
        uint16_t z =
            tft.getTouchRawZ();

        if (
            z <
            CALIBRATION_Z_THRESHOLD
        )
        {
            // Si l'utilisateur retire le doigt
            // avant d'avoir assez de mesures,
            // on recommence.
            if (
                samples > 0
            )
            {
                return false;
            }

            yield();
            delay(5);

            if (
                millis() - start >
                5000
            )
            {
                return false;
            }

            continue;
        }

        uint16_t x = 0;
        uint16_t y = 0;

        tft.getTouchRaw(
            &x,
            &y
        );

        // ----------------------------------------------------
        // Première mesure
        // ----------------------------------------------------

        if (
            samples == 0
        )
        {
            stableX = x;
            stableY = y;
        }
        else
        {
            if (
                abs(
                    (int32_t)x -
                    (int32_t)stableX
                ) >
                STABLE_TOLERANCE
                ||
                abs(
                    (int32_t)y -
                    (int32_t)stableY
                ) >
                STABLE_TOLERANCE
            )
            {
                Serial.println(
                    "[TOUCH] Mesure instable"
                );

                return false;
            }
        }

        sumX += x;
        sumY += y;

        samples++;

        delay(8);
        yield();
    }

    resultX =
        (uint16_t)(
            sumX /
            CALIBRATION_SAMPLES
        );

    resultY =
        (uint16_t)(
            sumY /
            CALIBRATION_SAMPLES
        );

    return true;
}

// ============================================================
// CALCUL CALIBRATION
//
// Reprend la logique de TFT_eSPI.
//
// On détecte automatiquement :
//
// - inversion X
// - inversion Y
// - échange X/Y
//
// Et surtout on stocke les DELTAS,
// pas les valeurs MAX.
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
    // Ordre :
    //
    // 0 = haut gauche
    // 1 = haut droit
    // 2 = bas droit
    // 3 = bas gauche
    // --------------------------------------------------------

    int32_t xTop =
        (
            (int32_t)rawX[0] +
            (int32_t)rawX[1]
        ) / 2;

    int32_t xBottom =
        (
            (int32_t)rawX[3] +
            (int32_t)rawX[2]
        ) / 2;

    int32_t yLeft =
        (
            (int32_t)rawY[0] +
            (int32_t)rawY[3]
        ) / 2;

    int32_t yRight =
        (
            (int32_t)rawY[1] +
            (int32_t)rawY[2]
        ) / 2;

    // --------------------------------------------------------
    // Détection axes
    //
    // Si X varie davantage entre haut/bas,
    // les axes tactiles sont probablement inversés.
    // --------------------------------------------------------

    int32_t verticalRawX =
        abs(
            (int32_t)rawX[0] -
            (int32_t)rawX[3]
        );

    int32_t horizontalRawX =
        abs(
            (int32_t)rawX[0] -
            (int32_t)rawX[1]
        );

    int32_t verticalRawY =
        abs(
            (int32_t)rawY[0] -
            (int32_t)rawY[3]
        );

    int32_t horizontalRawY =
        abs(
            (int32_t)rawY[0] -
            (int32_t)rawY[1]
        );

    Serial.print(
        "[TOUCH] Delta X horizontal = "
    );

    Serial.println(
        horizontalRawX
    );

    Serial.print(
        "[TOUCH] Delta X vertical = "
    );

    Serial.println(
        verticalRawX
    );

    Serial.print(
        "[TOUCH] Delta Y horizontal = "
    );

    Serial.println(
        horizontalRawY
    );

    Serial.print(
        "[TOUCH] Delta Y vertical = "
    );

    Serial.println(
        verticalRawY
    );

    bool rotateAxes =
        (
            verticalRawX >
            horizontalRawX
        );

    // --------------------------------------------------------
    // Valeurs de travail
    // --------------------------------------------------------

    int32_t touchX0;
    int32_t touchX1;

    int32_t touchY0;
    int32_t touchY1;

    bool invertX = false;
    bool invertY = false;

    if (
        rotateAxes
    )
    {
        // ----------------------------------------------------
        // Axe X écran = RAW Y
        // Axe Y écran = RAW X
        // ----------------------------------------------------

        touchX0 =
            (
                (int32_t)rawY[0] +
                (int32_t)rawY[3]
            ) / 2;

        touchX1 =
            (
                (int32_t)rawY[1] +
                (int32_t)rawY[2]
            ) / 2;

        touchY0 =
            (
                (int32_t)rawX[0] +
                (int32_t)rawX[1]
            ) / 2;

        touchY1 =
            (
                (int32_t)rawX[3] +
                (int32_t)rawX[2]
            ) / 2;
    }
    else
    {
        // ----------------------------------------------------
        // Axe X écran = RAW X
        // Axe Y écran = RAW Y
        // ----------------------------------------------------

        touchX0 =
            (
                (int32_t)rawX[0] +
                (int32_t)rawX[3]
            ) / 2;

        touchX1 =
            (
                (int32_t)rawX[1] +
                (int32_t)rawX[2]
            ) / 2;

        touchY0 =
            (
                (int32_t)rawY[0] +
                (int32_t)rawY[1]
            ) / 2;

        touchY1 =
            (
                (int32_t)rawY[3] +
                (int32_t)rawY[2]
            ) / 2;
    }

    // --------------------------------------------------------
    // Inversion X
    // --------------------------------------------------------

    if (
        touchX0 >
        touchX1
    )
    {
        int32_t tmp =
            touchX0;

        touchX0 =
            touchX1;

        touchX1 =
            tmp;

        invertX =
            true;
    }

    // --------------------------------------------------------
    // Inversion Y
    // --------------------------------------------------------

    if (
        touchY0 >
        touchY1
    )
    {
        int32_t tmp =
            touchY0;

        touchY0 =
            touchY1;

        touchY1 =
            tmp;

        invertY =
            true;
    }

    // --------------------------------------------------------
    // Les valeurs TFT_eSPI sont des DELTAS
    // --------------------------------------------------------

    int32_t deltaX =
        touchX1 -
        touchX0;

    int32_t deltaY =
        touchY1 -
        touchY0;

    Serial.println();

    Serial.print(
        "[TOUCH] Axes inverses = "
    );

    Serial.println(
        rotateAxes ? "OUI" : "NON"
    );

    Serial.print(
        "[TOUCH] Inversion X = "
    );

    Serial.println(
        invertX ? "OUI" : "NON"
    );

    Serial.print(
        "[TOUCH] Inversion Y = "
    );

    Serial.println(
        invertY ? "OUI" : "NON"
    );

    Serial.print(
        "[TOUCH] X0 = "
    );

    Serial.println(
        touchX0
    );

    Serial.print(
        "[TOUCH] X1 = "
    );

    Serial.println(
        deltaX
    );

    Serial.print(
        "[TOUCH] Y0 = "
    );

    Serial.println(
        touchY0
    );

    Serial.print(
        "[TOUCH] Y1 = "
    );

    Serial.println(
        deltaY
    );

    // --------------------------------------------------------
    // VALIDATION
    // --------------------------------------------------------

    if (
        deltaX < 300
        ||
        deltaY < 300
    )
    {
        Serial.println();
        Serial.println(
            "[TOUCH] ERREUR : amplitude RAW insuffisante"
        );

        Serial.println(
            "[TOUCH] Calibration REFUSEE"
        );

        Serial.println(
            "[TOUCH] Recommencer les 4 coins"
        );

        return false;
    }

    if (
        touchX0 < 10
        ||
        touchX0 > 4080
        ||
        touchY0 < 10
        ||
        touchY0 > 4080
    )
    {
        Serial.println(
            "[TOUCH] ATTENTION : origine RAW inhabituelle"
        );
    }

    // --------------------------------------------------------
    // FORMAT TFT_eSPI
    // --------------------------------------------------------

    calData[0] =
        (uint16_t)touchX0;

    calData[1] =
        (uint16_t)deltaX;

    calData[2] =
        (uint16_t)touchY0;

    calData[3] =
        (uint16_t)deltaY;

    calData[4] =
        0;

    if (
        rotateAxes
    )
    {
        calData[4] |= 0x01;
    }

    if (
        invertX
    )
    {
        calData[4] |= 0x02;
    }

    if (
        invertY
    )
    {
        calData[4] |= 0x04;
    }

    // --------------------------------------------------------
    // APPLICATION
    // --------------------------------------------------------

    tft.setTouch(
        calData
    );

    Serial.println();

    Serial.println(
        "[TOUCH] Parametres TFT_eSPI appliques"
    );

    Serial.print(
        "[TOUCH] calData[0] = "
    );

    Serial.println(
        calData[0]
    );

    Serial.print(
        "[TOUCH] calData[1] = "
    );

    Serial.println(
        calData[1]
    );

    Serial.print(
        "[TOUCH] calData[2] = "
    );

    Serial.println(
        calData[2]
    );

    Serial.print(
        "[TOUCH] calData[3] = "
    );

    Serial.println(
        calData[3]
    );

    Serial.print(
        "[TOUCH] calData[4] = "
    );

    Serial.println(
        calData[4]
    );

    return true;
}

// ============================================================
// INIT
// ============================================================

void touchCalibrationInit()
{
    Serial.println(
        "[TOUCH] Calibration Init"
    );

    Serial.print(
        "[TOUCH] FORCE_TOUCH_CALIBRATION = "
    );

    Serial.println(
        FORCE_TOUCH_CALIBRATION
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

    stableX = 0;
    stableY = 0;

    stableCount = 0;

    touchWasDetected = false;

    lastTouchTime =
        millis();

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

    tft.setRotation(
        SCREEN_ROTATION
    );

    tft.fillScreen(
        COLOR_BLACK
    );

    drawCalibrationScreen();

    // --------------------------------------------------------
    // IMPORTANT
    //
    // On laisse le temps à l'utilisateur
    // de retirer le doigt éventuellement posé.
    // --------------------------------------------------------

    delay(
        CALIBRATION_START_DELAY
    );

    waitRawTouchRelease();

    stableCount = 0;

    touchWasDetected = false;

    lastTouchTime =
        millis();

    Serial.println();

    Serial.println(
        "[TOUCH] Calibration tactile"
    );

    Serial.println(
        "[TOUCH] Aucun toucher automatique"
    );

    Serial.println(
        "[TOUCH] Attente toucher coin 1"
    );
}

// ============================================================
// UPDATE
// ============================================================

void updateTouchCalibration()
{
    // ========================================================
    // COINS
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
        // ----------------------------------------------------
        // DEBOUNCE
        // ----------------------------------------------------

        if (
            touchWasDetected
            &&
            millis() - lastTouchTime <
            TOUCH_DEBOUNCE_TIME
        )
        {
            return;
        }

        uint8_t index = 0;

        if (
            calibrationState ==
            CALIBRATION_CORNER_1
        )
        {
            index = 0;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_2
        )
        {
            index = 1;
        }
        else if (
            calibrationState ==
            CALIBRATION_CORNER_3
        )
        {
            index = 2;
        }
        else
        {
            index = 3;
        }

        // ----------------------------------------------------
        // CAPTURE
        // ----------------------------------------------------

        uint16_t x = 0;
        uint16_t y = 0;

        if (
            !captureCalibrationPoint(
                x,
                y
            )
        )
        {
            return;
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

        Serial.print(
            " valide : X="
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
        // ATTENDRE RELACHEMENT
        // ----------------------------------------------------

        waitRawTouchRelease();

        stableCount = 0;

        touchWasDetected = false;

        lastTouchTime =
            millis();

        // ----------------------------------------------------
        // ETAPE SUIVANTE
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
            // 4 COINS
            // ------------------------------------------------

            if (
                !calculateTouchCalibration()
            )
            {
                calibrationState =
                    CALIBRATION_CORNER_1;

                drawCalibrationScreen();

                delay(200);

                return;
            }

            calibrationState =
                CALIBRATION_ORIENTATION;

            drawOrientationScreen();

            delay(300);

            stableCount = 0;

            touchWasDetected = false;

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
        // AFFICHER ETAPE SUIVANTE
        // ----------------------------------------------------

        drawCalibrationScreen();

        delay(200);

        return;
    }

    // ========================================================
    // ORIENTATION
    // ========================================================

    if (
        calibrationState ==
        CALIBRATION_ORIENTATION
    )
    {
        uint16_t x = 0;
        uint16_t y = 0;

        // ----------------------------------------------------
        // getTouch() est maintenant utilisable car
        // calData est correctement construit.
        // ----------------------------------------------------

        if (
            !tft.getTouch(
                &x,
                &y,
                TOUCH_THRESHOLD
            )
        )
        {
            return;
        }

        Serial.print(
            "[ORIENTATION TOUCH] X="
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
        // ORIENTATION 0
        // ----------------------------------------------------

        if (
            x >= 10
            &&
            x <= SCREEN_WIDTH - 10
            &&
            y >= 80
            &&
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
        // ORIENTATION 2
        // ----------------------------------------------------

        else if (
            x >= 10
            &&
            x <= SCREEN_WIDTH - 10
            &&
            y >= 195
            &&
            y <= 310
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
        // RELACHEMENT
        // ----------------------------------------------------

        waitRawTouchRelease();

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
        // SAUVEGARDE
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
// ETAT ACTIF
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

// ============================================================
// CALIBRATION TERMINEE
// ============================================================

bool touchCalibrationFinished()
{
    return (
        calibrationState ==
        CALIBRATION_DONE
    );
}

// ============================================================
// ROTATION
// ============================================================

uint8_t selectedTouchRotation()
{
    return selectedRotation;
}

// ============================================================
// CHOIX ORIENTATION
// ============================================================

bool touchCalibrationChoice()
{
    return (
        calibrationState ==
        CALIBRATION_ORIENTATION
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
        Serial.println(
            "[TOUCH] EEPROM magic invalide"
        );

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
        Serial.println(
            "[TOUCH] EEPROM version invalide"
        );

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
        Serial.println(
            "[TOUCH] EEPROM resolution invalide"
        );

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
        Serial.println(
            "[TOUCH] EEPROM rotation invalide"
        );

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
        Serial.println(
            "[TOUCH] EEPROM checksum invalide"
        );

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

    Serial.println(
        "[TOUCH] Calibration EEPROM chargee"
    );

    Serial.print(
        "[TOUCH] Rotation = "
    );

    Serial.println(
        selectedRotation
    );

    Serial.print(
        "[TOUCH] calData = "
    );

    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        Serial.print(
            calData[i]
        );

        if (
            i < 4
        )
        {
            Serial.print(
                ", "
            );
        }
    }

    Serial.println();

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

bool loadPreviousCalibration()
{
    return loadTouchCalibration();
}