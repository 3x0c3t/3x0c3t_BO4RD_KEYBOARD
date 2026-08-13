#include "keyboard.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT INTERNE
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static char keyboardText[KEYBOARD_MAX_TEXT];

static uint8_t keyboardTextLength = 0;

static bool validated = false;

// ============================================================
// ANTI-REBOND
// ============================================================

static unsigned long lastTouchTime = 0;

static const unsigned long TOUCH_DELAY = 180;

// ============================================================
// DIMENSIONS
// ============================================================

static const int16_t HEADER_HEIGHT = 42;

static const int16_t KEY_ROWS = 4;

static const int16_t KEY_COLS = 10;

static const int16_t KEY_Y =
    KEYBOARD_Y + HEADER_HEIGHT;

static const int16_t KEY_HEIGHT =
    (SCREEN_HEIGHT - KEY_Y) / KEY_ROWS;

static const int16_t KEY_WIDTH =
    SCREEN_WIDTH / KEY_COLS;

// ============================================================
// CLAVIER ALPHABETIQUE
// ============================================================

static const char alphaRows[4][11] =
{
    "QWERTYUIOP",
    "ASDFGHJKL",
    "ZXCVBNM",
    "1234567890"
};

// ============================================================
// CLAVIER NUMERIQUE
// ============================================================

static const char numericRows[4][11] =
{
    "1234567890",
    "-+*/=()<>",
    ".,:;!?%#",
    "ABCDEFGHIJ"
};

// ============================================================
// EFFACER TEXTE
// ============================================================

static void clearText()
{
    keyboardTextLength = 0;

    keyboardText[0] = '\0';
}

// ============================================================
// AJOUTER CARACTERE
// ============================================================

static void addCharacter(
    char c
)
{
    if (
        keyboardTextLength >=
        KEYBOARD_MAX_TEXT - 1
    )
    {
        return;
    }

    keyboardText[keyboardTextLength] = c;

    keyboardTextLength++;

    keyboardText[keyboardTextLength] = '\0';
}

// ============================================================
// BACKSPACE
// ============================================================

static void backspaceCharacter()
{
    if (keyboardTextLength == 0)
        return;

    keyboardTextLength--;

    keyboardText[keyboardTextLength] = '\0';
}

// ============================================================
// DESSIN TOUCHE
// ============================================================

static void drawKey(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const char* label
)
{
    tft.drawRect(
        x,
        y,
        w,
        h,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        label,
        x + w / 2,
        y + h / 2,
        2
    );

    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    // --------------------------------------------------------
    // Fond
    // --------------------------------------------------------

    tft.fillScreen(TFT_BLACK);

    // --------------------------------------------------------
    // Barre supérieure
    // --------------------------------------------------------

    tft.fillRect(
        0,
        0,
        SCREEN_WIDTH,
        HEADER_HEIGHT,
        TFT_DARKGREY
    );

    tft.drawRect(
        0,
        0,
        SCREEN_WIDTH,
        HEADER_HEIGHT,
        TFT_WHITE
    );

    // --------------------------------------------------------
    // Texte
    // --------------------------------------------------------

    tft.setTextColor(
        TFT_WHITE,
        TFT_DARKGREY
    );

    tft.setTextDatum(TL_DATUM);

    tft.drawString(
        keyboardText,
        6,
        8,
        2
    );

    // --------------------------------------------------------
    // ABC / 123
    // --------------------------------------------------------

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        currentMode == KEYBOARD_ALPHA
            ? "ABC"
            : "123",
        25,
        HEADER_HEIGHT - 10,
        1
    );

    // --------------------------------------------------------
    // Backspace
    // --------------------------------------------------------

    tft.drawString(
        "<",
        190,
        HEADER_HEIGHT - 10,
        2
    );

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    tft.drawString(
        "OK",
        225,
        HEADER_HEIGHT - 10,
        1
    );

    tft.setTextDatum(TL_DATUM);

    // --------------------------------------------------------
    // Clavier actif
    // --------------------------------------------------------

    const char (*rows)[11];

    if (currentMode == KEYBOARD_ALPHA)
    {
        rows = alphaRows;
    }
    else
    {
        rows = numericRows;
    }

    // --------------------------------------------------------
    // Touches
    // --------------------------------------------------------

    for (
        int row = 0;
        row < KEY_ROWS;
        row++
    )
    {
        for (
            int col = 0;
            col < KEY_COLS;
            col++
        )
        {
            int16_t x =
                col * KEY_WIDTH;

            int16_t y =
                KEY_Y +
                row * KEY_HEIGHT;

            char label[2];

            label[0] =
                rows[row][col];

            label[1] =
                '\0';

            drawKey(
                x,
                y,
                KEY_WIDTH,
                KEY_HEIGHT,
                label
            );
        }
    }
}

// ============================================================
// INITIALISATION
// ============================================================

void keyboardBegin()
{
    Serial.println(
        "[KEYBOARD] Initialisation"
    );

    clearText();

    validated = false;

    currentMode =
        KEYBOARD_ALPHA;

    lastTouchTime =
        millis();

    Serial.println(
        "[KEYBOARD] Dessin clavier"
    );

    keyboardDraw();

    Serial.println(
        "[KEYBOARD] Pret"
    );
}

// ============================================================
// TEST ZONE
// ============================================================

static bool touchInside(
    int16_t x,
    int16_t y,
    int16_t bx,
    int16_t by,
    int16_t bw,
    int16_t bh
)
{
    return
        x >= bx &&
        x < bx + bw &&
        y >= by &&
        y < by + bh;
}

// ============================================================
// TRAITEMENT TACTILE
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    // --------------------------------------------------------
    // Coordonnées invalides
    // --------------------------------------------------------

    if (
        x < 0 ||
        x >= SCREEN_WIDTH ||
        y < 0 ||
        y >= SCREEN_HEIGHT
    )
    {
        return;
    }

    // --------------------------------------------------------
    // Anti-rebond
    // --------------------------------------------------------

    unsigned long now =
        millis();

    if (
        now - lastTouchTime <
        TOUCH_DELAY
    )
    {
        return;
    }

    lastTouchTime = now;

    // --------------------------------------------------------
    // BARRE SUPERIEURE
    // --------------------------------------------------------

    if (y < HEADER_HEIGHT)
    {
        // ----------------------------------------------------
        // ABC / 123
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                0,
                0,
                55,
                HEADER_HEIGHT
            )
        )
        {
            if (
                currentMode ==
                KEYBOARD_ALPHA
            )
            {
                currentMode =
                    KEYBOARD_NUMERIC;
            }
            else
            {
                currentMode =
                    KEYBOARD_ALPHA;
            }

            Serial.println(
                "[KEYBOARD] Mode change"
            );

            keyboardDraw();

            return;
        }

        // ----------------------------------------------------
        // BACKSPACE
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                165,
                0,
                40,
                HEADER_HEIGHT
            )
        )
        {
            backspaceCharacter();

            Serial.println(
                "[KEYBOARD] Backspace"
            );

            keyboardDraw();

            return;
        }

        // ----------------------------------------------------
        // OK
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                205,
                0,
                35,
                HEADER_HEIGHT
            )
        )
        {
            if (
                keyboardTextLength == 0
            )
            {
                Serial.println(
                    "[KEYBOARD] OK ignore : texte vide"
                );

                return;
            }

            validated = true;

            Serial.print(
                "[KEYBOARD] Validation : "
            );

            Serial.println(
                keyboardText
            );

            return;
        }

        return;
    }

    // --------------------------------------------------------
    // AVANT LES TOUCHES
    // --------------------------------------------------------

    if (y < KEY_Y)
        return;

    // --------------------------------------------------------
    // LIGNE / COLONNE
    // --------------------------------------------------------

    int row =
        (y - KEY_Y) /
        KEY_HEIGHT;

    int col =
        x / KEY_WIDTH;

    if (
        row < 0 ||
        row >= KEY_ROWS
    )
    {
        return;
    }

    if (
        col < 0 ||
        col >= KEY_COLS
    )
    {
        return;
    }

    // --------------------------------------------------------
    // CLAVIER ACTIF
    // --------------------------------------------------------

    const char (*rows)[11];

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        rows = alphaRows;
    }
    else
    {
        rows = numericRows;
    }

    char c =
        rows[row][col];

    if (c == '\0')
        return;

    // --------------------------------------------------------
    // AJOUT
    // --------------------------------------------------------

    addCharacter(c);

    Serial.print(
        "[KEYBOARD] Touche : "
    );

    Serial.println(c);

    keyboardDraw();
}

// ============================================================
// MODE
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
)
{
    currentMode = mode;

    keyboardDraw();
}

KeyboardMode keyboardGetMode()
{
    return currentMode;
}

// ============================================================
// TEXTE
// ============================================================

const char* keyboardGetText()
{
    return keyboardText;
}

// ============================================================
// VALIDATION
// ============================================================

bool keyboardWasValidated()
{
    return validated;
}

void keyboardClearValidated()
{
    validated = false;
}