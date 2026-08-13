#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static KeyboardMode currentMode =
    KEYBOARD_ALPHA;

static String keyboardText;

// ============================================================
// TOUCHES ALPHA
// ============================================================

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

// ============================================================
// TOUCHES NUMERIQUES
// ============================================================

static const char* numericKeys[KEYBOARD_ROWS][KEYBOARD_COLS] =
{
    {
        "1", "2", "3", "4", "5",
        "6", "7", "8", "9", "0"
    },

    {
        "-", "+", "*", "/", "=",
        ".", ",", ":", ";", "_"
    },

    {
        "(", ")", "[", "]", "{",
        "}", "<", ">", "%", "#"
    },

    {
        "@", "!", "?", "$", "&",
        "'", "\"", " ", " ", " "
    }
};

// ============================================================
// TOUCHES SYMBOLS
// ============================================================

static const char* symbolKeys[KEYBOARD_ROWS][KEYBOARD_COLS] =
{
    {
        "!", "@", "#", "$", "%",
        "^", "&", "*", "(", ")"
    },

    {
        "-", "_", "+", "=",
        "/", "\\", "|", "<", ">",
        "~"
    },

    {
        "[", "]", "{", "}", "(",
        ")", "'", "\"", "`", "."
    },

    {
        ",", ";", ":", "?",
        " ", " ", " ", " ", " ", " "
    }
};

// ============================================================
// DIMENSIONS
// ============================================================

static int16_t keyWidth()
{
    return KEYBOARD_W /
           KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_HEIGHT /
           KEYBOARD_ROWS;
}

// ============================================================
// RECUPERATION TOUCHE
// ============================================================

static const char* getKey(
    uint8_t row,
    uint8_t col
)
{
    if (
        row >= KEYBOARD_ROWS ||
        col >= KEYBOARD_COLS
    )
    {
        return "";
    }

    switch (currentMode)
    {
        case KEYBOARD_ALPHA:
            return alphaKeys[row][col];

        case KEYBOARD_NUMERIC:
            return numericKeys[row][col];

        case KEYBOARD_SYMBOLS:
            return symbolKeys[row][col];

        default:
            return "";
    }
}

// ============================================================
// ZONE TEXTE
// ============================================================

static void drawInputArea()
{
    tft.fillRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_BLACK
    );

    tft.drawRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_GREY
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(
        KEYBOARD_TEXT_SIZE
    );

    tft.setCursor(
        INPUT_X + 8,
        INPUT_Y + 12
    );

    tft.print(
        keyboardText
    );
}

// ============================================================
// DELETE
// ============================================================

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
        BTN_DEL_X + 23,
        BTN_DEL_Y + 13
    );

    tft.print("<");
}

// ============================================================
// OK
// ============================================================

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
        BTN_OK_X + 13,
        BTN_OK_Y + 13
    );

    tft.print("OK");
}

// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    tft.fillScreen(
        COLOR_BLACK
    );

    drawInputArea();

    drawDeleteButton();

    const int16_t width =
        keyWidth();

    const int16_t height =
        keyHeight();

    for (
        uint8_t row = 0;
        row < KEYBOARD_ROWS;
        row++
    )
    {
        for (
            uint8_t col = 0;
            col < KEYBOARD_COLS;
            col++
        )
        {
            const int16_t x =
                KEYBOARD_X +
                col * width;

            const int16_t y =
                KEYBOARD_Y +
                row * height;

            tft.fillRect(
                x + 1,
                y + 1,
                width - 2,
                height - 2,
                COLOR_GREY
            );

            tft.drawRect(
                x,
                y,
                width,
                height,
                COLOR_WHITE
            );

            const char* key =
                getKey(
                    row,
                    col
                );

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_GREY
            );

            tft.setTextSize(2);

            const int16_t textWidth =
                strlen(key) * 12;

            const int16_t tx =
                x +
                (width - textWidth) / 2;

            const int16_t ty =
                y +
                (height - 16) / 2;

            tft.setCursor(
                tx,
                ty
            );

            tft.print(
                key
            );
        }
    }

    drawOKButton();
}

// ============================================================
// INITIALISATION
// ============================================================

void keyboardInit()
{
    Serial.println(
        "[KEYBOARD] Init"
    );

    keyboardText = "";

    currentMode =
        KEYBOARD_ALPHA;

    Serial.println(
        "[KEYBOARD] OK"
    );
}

void keyboardBegin()
{
    keyboardInit();

    keyboardDraw();
}

// ============================================================
// DELETE
// ============================================================

void keyboardDelete()
{
    if (
        keyboardText.length() > 0
    )
    {
        keyboardText.remove(
            keyboardText.length() - 1
        );
    }

    drawInputArea();

    Serial.print(
        "[KEYBOARD] TEXTE = \""
    );

    Serial.print(
        keyboardText
    );

    Serial.println(
        "\""
    );
}

// ============================================================
// VALIDATION
// ============================================================

void keyboardValidate()
{
    Serial.print(
        "[KEYBOARD] VALIDATION = "
    );

    Serial.println(
        keyboardText
    );
}

// ============================================================
// CLEAR
// ============================================================

void keyboardClear()
{
    keyboardText = "";

    drawInputArea();
}

// ============================================================
// TOUCH CLAVIER
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print(
        "[KEYBOARD] X="
    );

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    // --------------------------------------------------------
    // DELETE
    // --------------------------------------------------------

    if (
        x >= BTN_DEL_X &&
        x < BTN_DEL_X + BTN_DEL_W &&
        y >= BTN_DEL_Y &&
        y < BTN_DEL_Y + BTN_DEL_H
    )
    {
        keyboardDelete();

        return;
    }

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    if (
        x >= BTN_OK_X &&
        x < BTN_OK_X + BTN_OK_W &&
        y >= BTN_OK_Y &&
        y < BTN_OK_Y + BTN_OK_H
    )
    {
        keyboardValidate();

        return;
    }

    // --------------------------------------------------------
    // CLAVIER
    // --------------------------------------------------------

    if (
        x < KEYBOARD_X ||
        x >= KEYBOARD_X + KEYBOARD_W ||
        y < KEYBOARD_Y ||
        y >= KEYBOARD_Y + KEYBOARD_HEIGHT
    )
    {
        return;
    }

    const int16_t width =
        keyWidth();

    const int16_t height =
        keyHeight();

    const uint8_t col =
        (
            x - KEYBOARD_X
        ) / width;

    const uint8_t row =
        (
            y - KEYBOARD_Y
        ) / height;

    if (
        row >= KEYBOARD_ROWS ||
        col >= KEYBOARD_COLS
    )
    {
        return;
    }

    const char* key =
        getKey(
            row,
            col
        );

    if (
        key == nullptr ||
        strlen(key) == 0
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

    Serial.print(col);

    Serial.print(
        " KEY="
    );

    Serial.println(key);

    if (
        keyboardText.length() <
        KEYBOARD_MAX_LENGTH
    )
    {
        keyboardText += key;
    }

    drawInputArea();
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

const String& keyboardGetText()
{
    return keyboardText;
}