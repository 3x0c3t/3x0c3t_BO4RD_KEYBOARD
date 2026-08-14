#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// ETAT
// ============================================================

static KeyboardMode currentMode =
    KEYBOARD_ALPHA;

static String keyboardText = "";

static uint16_t cursorPosition = 0;

// ============================================================
// TOUCHES ALPHABETIQUES
// ============================================================
//
// Ligne 1 : A-I + SYM
// Ligne 2 : J-R
// Ligne 3 : S-Z
//
// Les cases vides restent volontairement disponibles.
//

static const char* alphaKeys[3][10] =
{
    {
        "A", "B", "C", "D", "E",
        "F", "G", "H", "I", "SYM"
    },

    {
        "J", "K", "L", "M", "N",
        "O", "P", "Q", "R", ""
    },

    {
        "S", "T", "U", "V", "W",
        "X", "Y", "Z", "", ""
    }
};

// ============================================================
// CHIFFRES
// ============================================================

static const char* numericKeys[10] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9"
};

// ============================================================
// SYMBOLES
// ============================================================

static const char* symbolKeys[3][10] =
{
    {
        "!", "@", "#", "$", "%",
        "&", "*", "+", "-", "="
    },

    {
        "(", ")", "[", "]", "{",
        "}", "<", ">", "/", "\\"
    },

    {
        "?", ".", ",", ":", ";",
        "'", "\"", "_", "~", "^"
    }
};

// ============================================================
// DIMENSIONS
// ============================================================

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_KEY_HEIGHT;
}

// ============================================================
// TEXTE
// ============================================================

static void drawInputText()
{
    int16_t textX = INPUT_TEXT_X;
    int16_t textY = INPUT_TEXT_Y;

    tft.fillRect(
        INPUT_TEXT_X,
        INPUT_TEXT_Y - 5,
        INPUT_TEXT_W,
        INPUT_TEXT_H,
        COLOR_BLACK
    );

    if (
        cursorPosition >
        keyboardText.length()
    )
    {
        cursorPosition =
            keyboardText.length();
    }

    String before =
        keyboardText.substring(
            0,
            cursorPosition
        );

    String after =
        keyboardText.substring(
            cursorPosition
        );

    // --------------------------------------------------------
    // Texte
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextSize(
        KEYBOARD_TEXT_SIZE
    );

    tft.setTextDatum(
        TL_DATUM
    );

    tft.drawString(
        before,
        textX,
        textY
    );

    // --------------------------------------------------------
    // Position curseur
    // --------------------------------------------------------

    int16_t charWidth = 6;

    int16_t cursorX =
        textX +
        before.length() *
        charWidth;

    if (
        cursorX <
        INPUT_TEXT_X +
        INPUT_TEXT_W
    )
    {
        tft.drawFastVLine(
            cursorX,
            INPUT_TEXT_Y - 2,
            24,
            COLOR_GREEN
        );
    }

    // --------------------------------------------------------
    // Texte après curseur
    // --------------------------------------------------------

    int16_t afterX =
        cursorX + 1;

    if (
        afterX <
        INPUT_TEXT_X +
        INPUT_TEXT_W
    )
    {
        tft.drawString(
            after,
            afterX,
            textY
        );
    }

    // --------------------------------------------------------
    // Position curseur
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_YELLOW,
        COLOR_BLACK
    );

    tft.setCursor(
        INPUT_TEXT_X,
        INPUT_Y + 42
    );

    tft.print(
        cursorPosition
    );

    tft.print(
        "/"
    );

    tft.print(
        keyboardText.length()
    );
}

// ============================================================
// BOUTON
// ============================================================

static void drawButton(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    uint16_t background,
    uint16_t foreground,
    const char* text
)
{
    tft.fillRect(
        x,
        y,
        w,
        h,
        background
    );

    tft.drawRect(
        x,
        y,
        w,
        h,
        COLOR_WHITE
    );

    tft.setTextColor(
        foreground,
        background
    );

    tft.setTextSize(
        1
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.drawString(
        text,
        x + w / 2,
        y + h / 2
    );

    tft.setTextDatum(
        TL_DATUM
    );
}

// ============================================================
// ZONE SAISIE
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
        COLOR_WHITE
    );

    // --------------------------------------------------------
    // Séparation texte / commandes
    // --------------------------------------------------------

    tft.drawFastVLine(
        147,
        INPUT_Y + 4,
        INPUT_H - 8,
        COLOR_GREY
    );

    // --------------------------------------------------------
    // Commandes
    // --------------------------------------------------------

    drawButton(
        BTN_DELETE_X,
        BTN_DELETE_Y,
        BTN_DELETE_W,
        BTN_DELETE_H,
        COLOR_RED,
        COLOR_WHITE,
        "X"
    );

    drawButton(
        BTN_LEFT_X,
        BTN_LEFT_Y,
        BTN_LEFT_W,
        BTN_LEFT_H,
        COLOR_BLUE,
        COLOR_WHITE,
        "<"
    );

    drawButton(
        BTN_RIGHT_X,
        BTN_RIGHT_Y,
        BTN_RIGHT_W,
        BTN_RIGHT_H,
        COLOR_BLUE,
        COLOR_WHITE,
        ">"
    );

    drawButton(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        COLOR_GREEN,
        COLOR_BLACK,
        "OK"
    );

    // --------------------------------------------------------
    // Texte
    // --------------------------------------------------------

    drawInputText();
}

// ============================================================
// TOUCHE CLAVIER
// ============================================================

static void drawKey(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const char* text
)
{
    tft.fillRect(
        x + 1,
        y + 1,
        w - 2,
        h - 2,
        COLOR_GREY
    );

    tft.drawRect(
        x,
        y,
        w,
        h,
        COLOR_WHITE
    );

    if (
        text == nullptr ||
        strlen(text) == 0
    )
    {
        return;
    }

    uint16_t background =
        COLOR_GREY;

    uint16_t foreground =
        COLOR_WHITE;

    if (
        strcmp(text, "SYM") == 0
    )
    {
        background =
            COLOR_MAGENTA;
    }

    tft.fillRect(
        x + 1,
        y + 1,
        w - 2,
        h - 2,
        background
    );

    tft.setTextColor(
        foreground,
        background
    );

    tft.setTextSize(
        1
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.drawString(
        text,
        x + w / 2,
        y + h / 2
    );

    tft.setTextDatum(
        TL_DATUM
    );
}

// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    tft.fillScreen(
        COLOR_BLACK
    );

    // ========================================================
    // ZONE SAISIE
    // ========================================================

    drawInputArea();

    // ========================================================
    // CLAVIER
    // ========================================================

    const int16_t width =
        keyWidth();

    const int16_t height =
        keyHeight();

    // ========================================================
    // MODE ALPHA
    // ========================================================

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        for (
            uint8_t row = 0;
            row < 3;
            row++
        )
        {
            for (
                uint8_t col = 0;
                col < KEYBOARD_COLS;
                col++
            )
            {
                int16_t x =
                    KEYBOARD_X +
                    col * width;

                int16_t y =
                    KEYBOARD_Y +
                    row * height;

                drawKey(
                    x,
                    y,
                    width,
                    height,
                    alphaKeys[row][col]
                );
            }
        }

        // ----------------------------------------------------
        // Ligne chiffres
        // ----------------------------------------------------

        for (
            uint8_t col = 0;
            col < 10;
            col++
        )
        {
            int16_t x =
                KEYBOARD_X +
                col * width;

            int16_t y =
                KEYBOARD_Y +
                3 * height;

            drawKey(
                x,
                y,
                width,
                height,
                numericKeys[col]
            );
        }
    }

    // ========================================================
    // MODE SYMBOLES
    // ========================================================

    else
    {
        for (
            uint8_t row = 0;
            row < 3;
            row++
        )
        {
            for (
                uint8_t col = 0;
                col < KEYBOARD_COLS;
                col++
            )
            {
                int16_t x =
                    KEYBOARD_X +
                    col * width;

                int16_t y =
                    KEYBOARD_Y +
                    row * height;

                drawKey(
                    x,
                    y,
                    width,
                    height,
                    symbolKeys[row][col]
                );
            }
        }

        // ----------------------------------------------------
        // Retour ALPHA
        // ----------------------------------------------------

        drawKey(
            KEYBOARD_X,
            KEYBOARD_Y,
            width,
            height,
            "ABC"
        );

        // ----------------------------------------------------
        // Ligne chiffres toujours disponible
        // ----------------------------------------------------

        for (
            uint8_t col = 0;
            col < 10;
            col++
        )
        {
            int16_t x =
                KEYBOARD_X +
                col * width;

            int16_t y =
                KEYBOARD_Y +
                3 * height;

            drawKey(
                x,
                y,
                width,
                height,
                numericKeys[col]
            );
        }
    }

    // ========================================================
    // INDICATEUR MODE
    // ========================================================

    tft.setTextColor(
        COLOR_CYAN,
        COLOR_BLACK
    );

    tft.setTextSize(
        1
    );

    tft.setTextDatum(
        TL_DATUM
    );

    tft.drawString(
        currentMode ==
        KEYBOARD_ALPHA
        ? "ABC"
        : "SYM",
        5,
        165
    );
}

// ============================================================
// INIT
// ============================================================

void keyboardInit()
{
    Serial.println(
        "[KEYBOARD] Init"
    );

    keyboardText =
        "";

    cursorPosition =
        0;

    currentMode =
        KEYBOARD_ALPHA;

    Serial.println(
        "[KEYBOARD] Mode ALPHA"
    );

    Serial.println(
        "[KEYBOARD] OK"
    );
}

// ============================================================
// BEGIN
// ============================================================

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
        keyboardText.length() == 0
    )
    {
        return;
    }

    if (
        cursorPosition == 0
    )
    {
        return;
    }

    keyboardText.remove(
        cursorPosition - 1,
        1
    );

    cursorPosition--;

    drawInputArea();

    Serial.println(
        "[KEYBOARD] DELETE"
    );

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
// CURSEUR GAUCHE
// ============================================================

void keyboardCursorLeft()
{
    if (
        cursorPosition > 0
    )
    {
        cursorPosition--;
    }

    drawInputArea();

    Serial.println(
        "[KEYBOARD] CURSEUR GAUCHE"
    );

    Serial.print(
        "[KEYBOARD] CURSEUR = "
    );

    Serial.println(
        cursorPosition
    );
}

// ============================================================
// CURSEUR DROITE
// ============================================================

void keyboardCursorRight()
{
    if (
        cursorPosition <
        keyboardText.length()
    )
    {
        cursorPosition++;
    }

    drawInputArea();

    Serial.println(
        "[KEYBOARD] CURSEUR DROITE"
    );

    Serial.print(
        "[KEYBOARD] CURSEUR = "
    );

    Serial.println(
        cursorPosition
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
    keyboardText =
        "";

    cursorPosition =
        0;

    drawInputArea();

    Serial.println(
        "[KEYBOARD] CLEAR"
    );
}

// ============================================================
// AJOUT CARACTERE
// ============================================================

static void addCharacter(
    const char* key
)
{
    if (
        key == nullptr ||
        strlen(key) == 0
    )
    {
        return;
    }

    if (
        strcmp(key, "SYM") == 0
    )
    {
        currentMode =
            KEYBOARD_SYMBOLS;

        keyboardDraw();

        Serial.println(
            "[KEYBOARD] MODE SYMBOLS"
        );

        return;
    }

    if (
        strcmp(key, "ABC") == 0
    )
    {
        currentMode =
            KEYBOARD_ALPHA;

        keyboardDraw();

        Serial.println(
            "[KEYBOARD] MODE ALPHA"
        );

        return;
    }

    if (
        keyboardText.length() >=
        KEYBOARD_MAX_LENGTH
    )
    {
        return;
    }

    String before =
        keyboardText.substring(
            0,
            cursorPosition
        );

    String after =
        keyboardText.substring(
            cursorPosition
        );

    keyboardText =
        before +
        String(key) +
        after;

    cursorPosition++;

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
// TOUCH
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print(
        "[KEYBOARD] X="
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

    // ========================================================
    // DELETE
    // ========================================================

    if (
        x >= BTN_DELETE_X &&
        x < BTN_DELETE_X + BTN_DELETE_W &&
        y >= BTN_DELETE_Y &&
        y < BTN_DELETE_Y + BTN_DELETE_H
    )
    {
        keyboardDelete();

        return;
    }

    // ========================================================
    // GAUCHE
    // ========================================================

    if (
        x >= BTN_LEFT_X &&
        x < BTN_LEFT_X + BTN_LEFT_W &&
        y >= BTN_LEFT_Y &&
        y < BTN_LEFT_Y + BTN_LEFT_H
    )
    {
        keyboardCursorLeft();

        return;
    }

    // ========================================================
    // DROITE
    // ========================================================

    if (
        x >= BTN_RIGHT_X &&
        x < BTN_RIGHT_X + BTN_RIGHT_W &&
        y >= BTN_RIGHT_Y &&
        y < BTN_RIGHT_Y + BTN_RIGHT_H
    )
    {
        keyboardCursorRight();

        return;
    }

    // ========================================================
    // OK
    // ========================================================

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

    // ========================================================
    // CLAVIER
    // ========================================================

    if (
        x < KEYBOARD_X ||
        x >= KEYBOARD_X + KEYBOARD_W ||
        y < KEYBOARD_Y ||
        y >= KEYBOARD_Y + KEYBOARD_HEIGHT
    )
    {
        Serial.println(
            "[KEYBOARD] Touch hors zone"
        );

        return;
    }

    const int16_t width =
        keyWidth();

    const int16_t height =
        keyHeight();

    uint8_t col =
        (
            x - KEYBOARD_X
        ) / width;

    uint8_t row =
        (
            y - KEYBOARD_Y
        ) / height;

    if (
        col >= KEYBOARD_COLS ||
        row >= KEYBOARD_ROWS
    )
    {
        return;
    }

    // ========================================================
    // MODE ALPHA
    // ========================================================

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        // ----------------------------------------------------
        // Trois lignes alpha
        // ----------------------------------------------------

        if (
            row < 3
        )
        {
            const char* key =
                alphaKeys[row][col];

            if (
                key == nullptr ||
                strlen(key) == 0
            )
            {
                return;
            }

            if (
                strcmp(key, "SYM") == 0
            )
            {
                currentMode =
                    KEYBOARD_SYMBOLS;

                Serial.println(
                    "[KEYBOARD] MODE SYMBOLS"
                );

                keyboardDraw();

                return;
            }

            Serial.print(
                "[KEYBOARD] LETTRE = "
            );

            Serial.println(
                key
            );

            addCharacter(
                key
            );

            return;
        }

        // ----------------------------------------------------
        // Ligne chiffres
        // ----------------------------------------------------

        if (
            row == 3
        )
        {
            const char* key =
                numericKeys[col];

            Serial.print(
                "[KEYBOARD] CHIFFRE = "
            );

            Serial.println(
                key
            );

            addCharacter(
                key
            );

            return;
        }
    }

    // ========================================================
    // MODE SYMBOLES
    // ========================================================

    else
    {
        // ----------------------------------------------------
        // ABC dans la première case
        // ----------------------------------------------------

        if (
            row == 0 &&
            col == 0
        )
        {
            currentMode =
                KEYBOARD_ALPHA;

            Serial.println(
                "[KEYBOARD] MODE ALPHA"
            );

            keyboardDraw();

            return;
        }

        // ----------------------------------------------------
        // Symboles
        // ----------------------------------------------------

        const char* key =
            symbolKeys[row][col];

        if (
            key == nullptr ||
            strlen(key) == 0
        )
        {
            return;
        }

        Serial.print(
            "[KEYBOARD] SYMBOLE = "
        );

        Serial.println(
            key
        );

        addCharacter(
            key
        );

        return;
    }
}

// ============================================================
// MODE
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
)
{
    currentMode =
        mode;

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