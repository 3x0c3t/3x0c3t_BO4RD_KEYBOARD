#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

/*
 * ============================================================
 * 3x0c3t BO4RD KEYBOARD
 * ============================================================
 */

// ------------------------------------------------------------
// ETAT DU CLAVIER
// ------------------------------------------------------------

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static String keyboardText = "";

// ------------------------------------------------------------
// LIBELLES DES TOUCHES
// ------------------------------------------------------------

static const char* alphaKeys[KEYBOARD_ROWS][KEYBOARD_COLS] =
{
    {
        "A", "B", "C", "D", "E",
        "F", "G", "H", "I", "J"
    },

    {
        "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T"
    },

    {
        "U", "V", "W", "X", "Y",
        "Z", "0", "1", "2", "3"
    },

    {
        "4", "5", "6", "7", "8",
        "9", "-", "_", ".", " "
    }
};

static const char* numericKeys[KEYBOARD_ROWS][KEYBOARD_COLS] =
{
    {
        "1", "2", "3", "4", "5",
        "6", "7", "8", "9", "0"
    },

    {
        "-", "/", ":", ";", ".",
        ",", "+", "*", "(", ")"
    },

    {
        "#", "@", "!", "?", "$",
        "%", "&", "=", "<", ">"
    },

    {
        "A", "B", "C", "D", "E",
        "F", "G", "H", "I", "J"
    }
};

static const char* symbolKeys[KEYBOARD_ROWS][KEYBOARD_COLS] =
{
    {
        "!", "@", "#", "$", "%",
        "^", "&", "*", "(", ")"
    },

    {
        "-", "_", "+", "=", "/",
        "\\", "|", "<", ">", "~"
    },

    {
        "[", "]", "{", "}", ":",
        ";", "'", "\"", ",", "."
    },

    {
        "0", "1", "2", "3", "4",
        "5", "6", "7", "8", "9"
    }
};

// ------------------------------------------------------------
// DIMENSIONS DES TOUCHES
// ------------------------------------------------------------

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_H / KEYBOARD_ROWS;
}

// ------------------------------------------------------------
// ZONE DE SAISIE
// ------------------------------------------------------------

static void drawInputArea()
{
    tft.fillRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_DARKGREY
    );

    tft.drawRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_DARKGREY
    );

    tft.setTextSize(2);

    tft.setCursor(
        INPUT_X + 8,
        INPUT_Y + 12
    );

    tft.print(keyboardText);
}

// ------------------------------------------------------------
// BOUTON DELETE
// ------------------------------------------------------------

static void drawDeleteButton()
{
    tft.fillRect(
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H,
        COLOR_RED
    );

    tft.drawRect(
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_RED
    );

    tft.setTextSize(2);

    tft.setCursor(
        BTN_DEL_X + 16,
        BTN_DEL_Y + 9
    );

    tft.print("DEL");
}

// ------------------------------------------------------------
// BOUTON OK
// ------------------------------------------------------------

static void drawOKButton()
{
    tft.fillRect(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        COLOR_GREEN
    );

    tft.drawRect(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_BLACK,
        COLOR_GREEN
    );

    tft.setTextSize(2);

    tft.setCursor(
        BTN_OK_X + 22,
        BTN_OK_Y + 9
    );

    tft.print("OK");
}

// ------------------------------------------------------------
// TEXTE D'UNE TOUCHE
// ------------------------------------------------------------

static const char* getKeyLabel(
    uint8_t row,
    uint8_t col
)
{
    if (row >= KEYBOARD_ROWS ||
        col >= KEYBOARD_COLS)
    {
        return "";
    }

    switch (currentMode)
    {
        case KEYBOARD_NUMERIC:
            return numericKeys[row][col];

        case KEYBOARD_SYMBOLS:
            return symbolKeys[row][col];

        case KEYBOARD_ALPHA:
        default:
            return alphaKeys[row][col];
    }
}

// ------------------------------------------------------------
// DESSIN CLAVIER
// ------------------------------------------------------------

void keyboardDraw()
{
    tft.fillRect(
        KEYBOARD_X,
        KEYBOARD_Y,
        KEYBOARD_W,
        KEYBOARD_H,
        COLOR_BLACK
    );

    int16_t kw = keyWidth();
    int16_t kh = keyHeight();

    for (uint8_t row = 0;
         row < KEYBOARD_ROWS;
         row++)
    {
        for (uint8_t col = 0;
             col < KEYBOARD_COLS;
             col++)
        {
            int16_t x =
                KEYBOARD_X +
                col * kw;

            int16_t y =
                KEYBOARD_Y +
                row * kh;

            tft.fillRect(
                x + 1,
                y + 1,
                kw - 2,
                kh - 2,
                COLOR_BLUE
            );

            tft.drawRect(
                x,
                y,
                kw,
                kh,
                COLOR_WHITE
            );

            const char* label =
                getKeyLabel(row, col);

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_BLUE
            );

            tft.setTextSize(1);

            int16_t textWidth =
                tft.textWidth(label);

            int16_t textX =
                x +
                (kw - textWidth) / 2;

            int16_t textY =
                y +
                (kh - 8) / 2;

            tft.setCursor(
                textX,
                textY
            );

            tft.print(label);
        }
    }

    drawInputArea();
    drawDeleteButton();
    drawOKButton();
}

// ------------------------------------------------------------
// INITIALISATION
// ------------------------------------------------------------

void keyboardInit()
{
    Serial.println("[KEYBOARD] Init");

    keyboardText = "";

    currentMode =
        KEYBOARD_ALPHA;

    keyboardDraw();

    Serial.println("[KEYBOARD] Init OK");
}

void keyboardBegin()
{
    keyboardInit();
}

// ------------------------------------------------------------
// DELETE
// ------------------------------------------------------------

void keyboardDelete()
{
    if (keyboardText.length() > 0)
    {
        keyboardText.remove(
            keyboardText.length() - 1
        );
    }

    Serial.print(
        "[KEYBOARD] DELETE -> "
    );

    Serial.println(
        keyboardText
    );

    drawInputArea();
}

// ------------------------------------------------------------
// VALIDATION
// ------------------------------------------------------------

void keyboardValidate()
{
    Serial.print(
        "[KEYBOARD] Validation : "
    );

    Serial.println(
        keyboardText
    );

    if (keyboardText.length() > 0)
    {
        Serial.print(
            "[KEYBOARD] Texte valide : "
        );

        Serial.println(
            keyboardText
        );
    }
}

// ------------------------------------------------------------
// CHANGEMENT DE MODE
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// RECUPERATION DU TEXTE
// ------------------------------------------------------------

const String& keyboardGetText()
{
    return keyboardText;
}

// ------------------------------------------------------------
// TRAITEMENT D'UNE TOUCHE
// ------------------------------------------------------------

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print(
        "[KEYBOARD] Analyse X="
    );

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    // --------------------------------------------------------
    // BOUTON DELETE
    // --------------------------------------------------------

    if (
        x >= BTN_DEL_X &&
        x < BTN_DEL_X + BTN_DEL_W &&
        y >= BTN_DEL_Y &&
        y < BTN_DEL_Y + BTN_DEL_H
    )
    {
        Serial.println(
            "[KEYBOARD] TOUCHE = DELETE"
        );

        keyboardDelete();

        return;
    }

    // --------------------------------------------------------
    // BOUTON OK
    // --------------------------------------------------------

    if (
        x >= BTN_OK_X &&
        x < BTN_OK_X + BTN_OK_W &&
        y >= BTN_OK_Y &&
        y < BTN_OK_Y + BTN_OK_H
    )
    {
        Serial.println(
            "[KEYBOARD] ZONE = OK"
        );

        keyboardValidate();

        return;
    }

    // --------------------------------------------------------
    // ZONE CLAVIER
    // --------------------------------------------------------

    if (
        x < KEYBOARD_X ||
        x >= KEYBOARD_X + KEYBOARD_W ||
        y < KEYBOARD_Y ||
        y >= KEYBOARD_Y + KEYBOARD_H
    )
    {
        Serial.println(
            "[KEYBOARD] Hors clavier"
        );

        return;
    }

    int16_t kw = keyWidth();
    int16_t kh = keyHeight();

    uint8_t col =
        (x - KEYBOARD_X) / kw;

    uint8_t row =
        (y - KEYBOARD_Y) / kh;

    if (
        row >= KEYBOARD_ROWS ||
        col >= KEYBOARD_COLS
    )
    {
        return;
    }

    Serial.print(
        "[KEYBOARD] ROW="
    );

    Serial.print(row);

    Serial.print(
        " COL="
    );

    Serial.println(col);

    const char* key =
        getKeyLabel(row, col);

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(key);

    // --------------------------------------------------------
    // ESPACE
    // --------------------------------------------------------

    if (
        strcmp(key, " ") == 0
    )
    {
        if (
            keyboardText.length() <
            KEYBOARD_MAX_LENGTH
        )
        {
            keyboardText += " ";
        }
    }

    // --------------------------------------------------------
    // AUTRES CARACTERES
    // --------------------------------------------------------

    else if (
        strlen(key) > 0
    )
    {
        if (
            keyboardText.length() <
            KEYBOARD_MAX_LENGTH
        )
        {
            keyboardText += key;
        }
    }

    Serial.print(
        "[KEYBOARD] TEXTE = \""
    );

    Serial.print(
        keyboardText
    );

    Serial.println("\"");

    drawInputArea();
}