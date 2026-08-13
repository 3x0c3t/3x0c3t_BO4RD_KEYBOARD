#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static String keyboardText = "";

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
        "-", "_", "+", "=", "/",
        "\\", "|", "<", ">", "~"
    },

    {
        "[", "]", "{", "}", "(",
        ")", "'", "\"", "`", "."
    },

    {
        ",", ";", ":", "?", " ",
        " ", " ", " ", " ", " "
    }
};

// ============================================================
// DIMENSIONS TOUCHES
// ============================================================

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_HEIGHT / KEYBOARD_ROWS;
}

// ============================================================
// TEXTE
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

    tft.setTextSize(KEYBOARD_TEXT_SIZE);

    tft.setCursor(
        INPUT_X + 8,
        INPUT_Y + 12
    );

    tft.print(keyboardText);
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
        BTN_DEL_X + 18,
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
        BTN_OK_X + 20,
        BTN_OK_Y + 13
    );

    tft.print("OK");
}

// ============================================================
// MODE
// ============================================================

static const char* getKey(
    uint8_t row,
    uint8_t col
)
{
    if (currentMode == KEYBOARD_ALPHA)
    {
        return alphaKeys[row][col];
    }

    if (currentMode == KEYBOARD_NUMERIC)
    {
        return numericKeys[row][col];
    }

    return symbolKeys[row][col];
}

// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    tft.fillScreen(COLOR_BLACK);

    drawInputArea();
    drawDeleteButton();

    int16_t kw = keyWidth();
    int16_t kh = keyHeight();

    for (uint8_t row = 0; row < KEYBOARD_ROWS; row++)
    {
        for (uint8_t col = 0; col < KEYBOARD_COLS; col++)
        {
            int16_t x = KEYBOARD_X + col * kw;
            int16_t y = KEYBOARD_Y + row * kh;

            tft.fillRect(
                x + 1,
                y + 1,
                kw - 2,
                kh - 2,
                COLOR_GREY
            );

            tft.drawRect(
                x,
                y,
                kw,
                kh,
                COLOR_WHITE
            );

            const char* key = getKey(row, col);

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_GREY
            );

            tft.setTextSize(2);

            int16_t tx =
                x + (kw / 2) -
                (strlen(key) * 6);

            int16_t ty =
                y + (kh / 2) - 8;

            if (strlen(key) == 0)
            {
                tx = x + kw / 2;
            }

            tft.setCursor(
                tx,
                ty
            );

            tft.print(key);
        }
    }

    drawOKButton();
}

// ============================================================
// INITIALISATION
// ============================================================

void keyboardInit()
{
    Serial.println("[KEYBOARD] Init");

    keyboardText = "";
    currentMode = KEYBOARD_ALPHA;

    Serial.println("[KEYBOARD] Init OK");
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
    if (keyboardText.length() > 0)
    {
        keyboardText.remove(
            keyboardText.length() - 1
        );
    }

    drawInputArea();

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");
}

// ============================================================
// VALIDATION
// ============================================================

void keyboardValidate()
{
    Serial.print("[KEYBOARD] Validation : ");
    Serial.println(keyboardText);

    if (keyboardText.length() > 0)
    {
        Serial.print("[KEYBOARD] Texte valide : ");
        Serial.println(keyboardText);
    }
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
// UPDATE
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print("[KEYBOARD] Analyse X=");
    Serial.print(x);
    Serial.print(" Y=");
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
        Serial.println("[KEYBOARD] TOUCHE = DELETE");

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
        Serial.println("[KEYBOARD] TOUCHE = OK");

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

    Serial.print("[KEYBOARD] ROW=");
    Serial.print(row);

    Serial.print(" COL=");
    Serial.println(col);

    const char* key =
        getKey(row, col);

    if (
        key == nullptr ||
        strlen(key) == 0
    )
    {
        return;
    }

    Serial.print("[KEYBOARD] TOUCHE = ");
    Serial.println(key);

    if (
        keyboardText.length() <
        KEYBOARD_MAX_LENGTH
    )
    {
        keyboardText += key;
    }

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");

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