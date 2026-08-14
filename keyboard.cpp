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
// 3 lignes exactement
//
// A B C D E F G H I
// J K L M N O P Q R
// S T U V W X Y Z
//
// La quatrième ligne est réservée aux chiffres.
// ============================================================

static const char* alphaKeys[3][10] =
{
    {
        "A", "B", "C", "D", "E",
        "F", "G", "H", "I", ""
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
// TOUCHES NUMERIQUES
// ============================================================
//
// 0 -> 9
//
// Dernière touche : SYM
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
// TOUCHES SPECIALES
// ============================================================

static const char* symbolKeys[10] =
{
    "!",
    "@",
    "#",
    "$",
    "%",
    "&",
    "*",
    "+",
    "-",
    "_"
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
    return KEYBOARD_HEIGHT / KEYBOARD_ROWS;
}

// ============================================================
// CALCUL TEXTE
// ============================================================

static void drawInputText()
{
    tft.fillRect(
        INPUT_X + 2,
        INPUT_Y + 2,
        INPUT_W - 4,
        INPUT_H - 4,
        COLOR_BLACK
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
        INPUT_Y + 17
    );

    // --------------------------------------------------------
    // Affichage avec curseur
    // --------------------------------------------------------

    String before = "";
    String after = "";

    if (
        cursorPosition >
        keyboardText.length()
    )
    {
        cursorPosition =
            keyboardText.length();
    }

    before =
        keyboardText.substring(
            0,
            cursorPosition
        );

    after =
        keyboardText.substring(
            cursorPosition
        );

    tft.print(
        before
    );

    // Curseur
    int16_t cursorX =
        INPUT_X +
        8 +
        before.length() * 12;

    tft.drawFastVLine(
        cursorX,
        INPUT_Y + 8,
        30,
        COLOR_GREEN
    );

    tft.print(
        after
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

    drawInputText();
}

// ============================================================
// BOUTON GENERIQUE
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

    tft.setTextSize(1);

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
// CONTROLES
// ============================================================

static void drawControls()
{
    drawButton(
        BTN_DELETE_X,
        BTN_DELETE_Y,
        BTN_DELETE_W,
        BTN_DELETE_H,
        COLOR_RED,
        COLOR_WHITE,
        "DEL"
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
}

// ============================================================
// TOUCHE SPECIALE
// ============================================================

static void drawSpecialButton(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h
)
{
    drawButton(
        x,
        y,
        w,
        h,
        COLOR_MAGENTA,
        COLOR_WHITE,
        "SYM"
    );
}

// ============================================================
// DESSIN D'UNE TOUCHE
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

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_GREY
    );

    tft.setTextSize(2);

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

    // --------------------------------------------------------
    // ZONE DE SAISIE
    // --------------------------------------------------------

    drawInputArea();

    // --------------------------------------------------------
    // CONTROLES
    // --------------------------------------------------------

    drawControls();

    // --------------------------------------------------------
    // CLAVIER
    // --------------------------------------------------------

    const int16_t width =
        keyWidth();

    const int16_t height =
        keyHeight();

    // --------------------------------------------------------
    // LIGNES ALPHA
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // LIGNE NUMERIQUE
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // MODE SYMBOLS
    // --------------------------------------------------------

    if (
        currentMode ==
        KEYBOARD_SYMBOLS
    )
    {
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
                2 * height;

            drawKey(
                x,
                y,
                width,
                height,
                symbolKeys[col]
            );
        }
    }

    // --------------------------------------------------------
    // INDICATION DU MODE
    // --------------------------------------------------------

    const char* modeText;

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        modeText = "ABC";
    }
    else
    {
        modeText = "SYM";
    }

    drawSpecialButton(
        0,
        KEYBOARD_Y + 3 * height,
        width,
        height
    );

    // --------------------------------------------------------
    // En mode normal, le premier emplacement est 0.
    // On redessine donc les chiffres 0-9.
    // --------------------------------------------------------

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

    // Bouton spécial par-dessus la première touche.
    drawSpecialButton(
        0,
        KEYBOARD_Y + 3 * height,
        width,
        height
    );

    // Indicateur
    tft.setTextColor(
        COLOR_YELLOW,
        COLOR_BLACK
    );

    tft.setTextSize(1);

    tft.setCursor(
        5,
        KEYBOARD_Y + 3 * height + height - 12
    );

    tft.print(
        modeText
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

    keyboardText = "";

    cursorPosition = 0;

    currentMode =
        KEYBOARD_ALPHA;

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
    keyboardText = "";

    cursorPosition = 0;

    drawInputArea();
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
        keyboardText.length() >=
        KEYBOARD_MAX_LENGTH
    )
    {
        return;
    }

    keyboardText =
        keyboardText.substring(
            0,
            cursorPosition
        )
        +
        String(key)
        +
        keyboardText.substring(
            cursorPosition
        );

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
        Serial.println(
            "[KEYBOARD] DELETE"
        );

        keyboardDelete();

        return;
    }

    // ========================================================
    // CURSEUR GAUCHE
    // ========================================================

    if (
        x >= BTN_LEFT_X &&
        x < BTN_LEFT_X + BTN_LEFT_W &&
        y >= BTN_LEFT_Y &&
        y < BTN_LEFT_Y + BTN_LEFT_H
    )
    {
        Serial.println(
            "[KEYBOARD] CURSEUR GAUCHE"
        );

        keyboardCursorLeft();

        return;
    }

    // ========================================================
    // CURSEUR DROITE
    // ========================================================

    if (
        x >= BTN_RIGHT_X &&
        x < BTN_RIGHT_X + BTN_RIGHT_W &&
        y >= BTN_RIGHT_Y &&
        y < BTN_RIGHT_Y + BTN_RIGHT_H
    )
    {
        Serial.println(
            "[KEYBOARD] CURSEUR DROITE"
        );

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
        Serial.println(
            "[KEYBOARD] OK"
        );

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
    // LIGNES ALPHA
    // ========================================================

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

    // ========================================================
    // LIGNE NUMERIQUE
    // ========================================================

    if (
        row == 3
    )
    {
        const char* key =
            numericKeys[col];

        // ----------------------------------------------------
        // Touche SYM
        // ----------------------------------------------------

        if (
            col == 0
        )
        {
            if (
                currentMode ==
                KEYBOARD_ALPHA
            )
            {
                currentMode =
                    KEYBOARD_SYMBOLS;
            }
            else
            {
                currentMode =
                    KEYBOARD_ALPHA;
            }

            Serial.println(
                "[KEYBOARD] MODE SYMBOLS"
            );

            keyboardDraw();

            return;
        }

        // ----------------------------------------------------
        // Chiffre
        // ----------------------------------------------------

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