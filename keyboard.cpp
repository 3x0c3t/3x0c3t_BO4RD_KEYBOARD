#include "keyboard.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static char keyboardText[KEYBOARD_MAX_TEXT];
static uint8_t keyboardTextLength = 0;

static bool validated = false;

static unsigned long lastTouchTime = 0;

static const unsigned long TOUCH_DELAY = 180;

// ============================================================
// DIMENSIONS
// ============================================================

static const int16_t INPUT_Y = 0;
static const int16_t INPUT_H = 42;

static const int16_t ACTION_WIDTH = 48;

static const int16_t DEL_X =
    SCREEN_WIDTH - (ACTION_WIDTH * 2);

static const int16_t OK_X =
    SCREEN_WIDTH - ACTION_WIDTH;

static const int16_t TEXT_W =
    DEL_X;

static const int16_t KEY_Y =
    KEYBOARD_Y;

static const int16_t KEY_ROWS = 4;
static const int16_t KEY_COLS = 10;

static const int16_t KEYBOARD_HEIGHT =
    SCREEN_HEIGHT - KEY_Y;

static const int16_t KEY_HEIGHT =
    KEYBOARD_HEIGHT / KEY_ROWS;

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
// CLEAR TEXT
// ============================================================

static void clearText()
{
    keyboardTextLength = 0;
    keyboardText[0] = '\0';
}

// ============================================================
// ADD CHARACTER
// ============================================================

static void addCharacter(char c)
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
    {
        return;
    }

    keyboardTextLength--;

    keyboardText[keyboardTextLength] = '\0';
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
// DESSIN BOUTON ACTION
// ============================================================

static void drawActionButton(
    int16_t x,
    const char* label
)
{
    tft.fillRect(
        x,
        INPUT_Y,
        ACTION_WIDTH,
        INPUT_H,
        TFT_DARKGREY
    );

    tft.drawRect(
        x,
        INPUT_Y,
        ACTION_WIDTH,
        INPUT_H,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_DARKGREY
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        label,
        x + ACTION_WIDTH / 2,
        INPUT_Y + INPUT_H / 2,
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
    // FOND
    // --------------------------------------------------------

    tft.fillScreen(TFT_BLACK);

    // --------------------------------------------------------
    // CHAMP DE SAISIE
    // --------------------------------------------------------

    tft.fillRect(
        0,
        INPUT_Y,
        TEXT_W,
        INPUT_H,
        TFT_DARKGREY
    );

    tft.drawRect(
        0,
        INPUT_Y,
        TEXT_W,
        INPUT_H,
        TFT_WHITE
    );

    // --------------------------------------------------------
    // TEXTE
    // --------------------------------------------------------

    tft.setTextColor(
        TFT_WHITE,
        TFT_DARKGREY
    );

    tft.setTextDatum(TL_DATUM);

    tft.drawString(
        keyboardText,
        6,
        11,
        2
    );

    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

    drawActionButton(
        DEL_X,
        "DEL"
    );

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    drawActionButton(
        OK_X,
        "OK"
    );

    // --------------------------------------------------------
    // CLAVIER
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

    for (int row = 0; row < KEY_ROWS; row++)
    {
        for (int col = 0; col < KEY_COLS; col++)
        {
            int16_t x =
                col * KEY_WIDTH;

            int16_t y =
                KEY_Y + row * KEY_HEIGHT;

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
    Serial.println("[KEYBOARD] Initialisation");

    clearText();

    validated = false;

    currentMode = KEYBOARD_ALPHA;

    lastTouchTime = millis();

    Serial.println(
        "[KEYBOARD] Dessin clavier"
    );

    keyboardDraw();

    Serial.println(
        "[KEYBOARD] Pret"
    );
}

// ============================================================
// MISE A JOUR
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    // --------------------------------------------------------
    // VALIDATION COORDONNEES
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
    // DEBUG
    // --------------------------------------------------------

    Serial.print(
        "[KEYBOARD] Analyse X="
    );

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    // --------------------------------------------------------
    // ANTI-REBOND
    // --------------------------------------------------------

    unsigned long now = millis();

    if (
        now - lastTouchTime <
        TOUCH_DELAY
    )
    {
        return;
    }

    lastTouchTime = now;

    // ========================================================
    // DEL
    // ========================================================

    if (
        touchInside(
            x,
            y,
            DEL_X,
            INPUT_Y,
            ACTION_WIDTH,
            INPUT_H
        )
    )
    {
        Serial.println(
            "[KEYBOARD] ZONE = DEL"
        );

        Serial.print(
            "[KEYBOARD] AVANT DEL = \""
        );

        Serial.print(
            keyboardText
        );

        Serial.println(
            "\""
        );

        backspaceCharacter();

        Serial.print(
            "[KEYBOARD] APRES DEL = \""
        );

        Serial.print(
            keyboardText
        );

        Serial.println(
            "\""
        );

        keyboardDraw();

        return;
    }

    // ========================================================
    // OK
    // ========================================================

    if (
        touchInside(
            x,
            y,
            OK_X,
            INPUT_Y,
            ACTION_WIDTH,
            INPUT_H
        )
    )
    {
        Serial.println(
            "[KEYBOARD] ZONE = OK"
        );

        if (keyboardTextLength == 0)
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

    // ========================================================
    // ZONE SUPERIEURE HORS BOUTONS
    // ========================================================

    if (y < KEY_Y)
    {
        Serial.println(
            "[KEYBOARD] ZONE = CHAMP"
        );

        return;
    }

    // ========================================================
    // CALCUL LIGNE
    // ========================================================

    int row =
        (y - KEY_Y) / KEY_HEIGHT;

    // ========================================================
    // CALCUL COLONNE
    // ========================================================

    int col =
        x / KEY_WIDTH;

    // --------------------------------------------------------
    // PROTECTION
    // --------------------------------------------------------

    if (
        row < 0 ||
        row >= KEY_ROWS ||
        col < 0 ||
        col >= KEY_COLS
    )
    {
        return;
    }

    // ========================================================
    // CLAVIER ACTIF
    // ========================================================

    const char (*rows)[11];

    if (currentMode == KEYBOARD_ALPHA)
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
    {
        return;
    }

    // ========================================================
    // DEBUG TOUCHE
    // ========================================================

    Serial.print(
        "[KEYBOARD] ROW="
    );

    Serial.print(row);

    Serial.print(
        " COL="
    );

    Serial.println(col);

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(c);

    // ========================================================
    // AJOUT
    // ========================================================

    addCharacter(c);

    Serial.print(
        "[KEYBOARD] TEXTE = \""
    );

    Serial.print(
        keyboardText
    );

    Serial.println(
        "\""
    );

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

// ============================================================
// LECTURE MODE
// ============================================================

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

// ============================================================
// CLEAR VALIDATION
// ============================================================

void keyboardClearValidated()
{
    validated = false;
}