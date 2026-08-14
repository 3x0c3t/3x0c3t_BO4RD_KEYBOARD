#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// CONFIGURATION CLAVIER
// ============================================================
//
// La taille de l'ecran vient uniquement de config.h.
// Aucune nouvelle declaration de SCREEN_WIDTH / SCREEN_HEIGHT.
//
// Ecran portrait : 240 x 320
//
// Zone texte
//   Y = 5
//   H = 42
//
// Clavier
//   Y = 52
//   H = jusqu'en bas de l'ecran
//
// 4 lignes :
//   1 : AZERTY
//   2 : QSDFG...
//   3 : WXCVB...
//   4 : commandes
//
// ============================================================

static const int16_t INPUT_MARGIN = 5;
static const int16_t INPUT_Y_POS = 5;
static const int16_t INPUT_HEIGHT = 42;

static const int16_t KEYBOARD_TOP = 52;
static const uint8_t KEYBOARD_ROWS_LOCAL = 4;
static const uint8_t KEYBOARD_COLS_LOCAL = 10;

static const int16_t KEY_GAP = 2;

// ============================================================
// ETAT
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static String keyboardText = "";

static bool shiftActive = false;

// ============================================================
// TOUCHES AZERTY
// ============================================================

static const char* alphaKeys[3][10] =
{
    {
        "A", "Z", "E", "R", "T",
        "Y", "U", "I", "O", "P"
    },

    {
        "Q", "S", "D", "F", "G",
        "H", "J", "K", "L", "M"
    },

    {
        "W", "X", "C", "V", "B",
        "N", ",", ".", "?", "!"
    }
};

// ============================================================
// TOUCHES NUMERIQUES
// ============================================================

static const char* numericKeys[3][10] =
{
    {
        "1", "2", "3", "4", "5",
        "6", "7", "8", "9", "0"
    },

    {
        "-", "+", "*", "/", "=",
        "(", ")", "[", "]", "%"
    },

    {
        "#", "@", "!", "?", ",",
        ".", ":", ";", "'", "\""
    }
};

// ============================================================
// DIMENSIONS
// ============================================================

static int16_t keyboardWidth()
{
    return SCREEN_WIDTH - (2 * INPUT_MARGIN);
}

static int16_t keyboardHeight()
{
    return SCREEN_HEIGHT - KEYBOARD_TOP;
}

static int16_t keyWidth()
{
    return keyboardWidth() / KEYBOARD_COLS_LOCAL;
}

static int16_t keyHeight()
{
    return keyboardHeight() / KEYBOARD_ROWS_LOCAL;
}

// ============================================================
// ZONE TEXTE
// ============================================================

static void drawInputArea()
{
    tft.fillRect(
        INPUT_MARGIN,
        INPUT_Y_POS,
        keyboardWidth(),
        INPUT_HEIGHT,
        COLOR_BLACK
    );

    tft.drawRect(
        INPUT_MARGIN,
        INPUT_Y_POS,
        keyboardWidth(),
        INPUT_HEIGHT,
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
        INPUT_MARGIN + 6,
        INPUT_Y_POS + 12
    );

    tft.print(
        keyboardText
    );
}

// ============================================================
// TEXTE DES TOUCHES
// ============================================================

static void drawKeyLabel(
    const char* label,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h
)
{
    if (
        label == nullptr ||
        strlen(label) == 0
    )
    {
        return;
    }

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_GREY
    );

    tft.setTextSize(2);

    int16_t textWidth =
        strlen(label) * 12;

    int16_t tx =
        x + (w - textWidth) / 2;

    int16_t ty =
        y + (h / 2) - 8;

    if (tx < x + 2)
    {
        tx = x + 2;
    }

    tft.setCursor(
        tx,
        ty
    );

    tft.print(
        label
    );
}

// ============================================================
// TOUCHE NORMALE
// ============================================================

static void drawNormalKey(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const char* label
)
{
    tft.fillRect(
        x + KEY_GAP,
        y + KEY_GAP,
        w - (KEY_GAP * 2),
        h - (KEY_GAP * 2),
        COLOR_GREY
    );

    tft.drawRect(
        x,
        y,
        w,
        h,
        COLOR_WHITE
    );

    drawKeyLabel(
        label,
        x,
        y,
        w,
        h
    );
}

// ============================================================
// TOUCHE SPECIALE
// ============================================================

static void drawSpecialKey(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const char* label,
    uint16_t color
)
{
    tft.fillRect(
        x + KEY_GAP,
        y + KEY_GAP,
        w - (KEY_GAP * 2),
        h - (KEY_GAP * 2),
        color
    );

    tft.drawRect(
        x,
        y,
        w,
        h,
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        color
    );

    tft.setTextSize(2);

    int16_t textWidth =
        strlen(label) * 12;

    int16_t tx =
        x + (w - textWidth) / 2;

    int16_t ty =
        y + (h / 2) - 8;

    if (tx < x + 2)
    {
        tx = x + 2;
    }

    tft.setCursor(
        tx,
        ty
    );

    tft.print(
        label
    );
}

// ============================================================
// DESSIN CLAVIER ALPHA
// ============================================================

static void drawAlphaKeyboard()
{
    int16_t kw = keyWidth();
    int16_t kh = keyHeight();

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
            col < KEYBOARD_COLS_LOCAL;
            col++
        )
        {
            int16_t x =
                col * kw;

            int16_t y =
                KEYBOARD_TOP +
                row * kh;

            drawNormalKey(
                x,
                y,
                kw,
                kh,
                alphaKeys[row][col]
            );
        }
    }

    // --------------------------------------------------------
    // DERNIERE LIGNE
    // --------------------------------------------------------

    int16_t y =
        KEYBOARD_TOP +
        3 * kh;

    // MAJ
    drawSpecialKey(
        0,
        y,
        kw * 2,
        kh,
        shiftActive ? "MAJ*" : "MAJ",
        COLOR_BLUE
    );

    // ESPACE
    drawSpecialKey(
        kw * 2,
        y,
        kw * 4,
        kh,
        "ESPACE",
        COLOR_GREY
    );

    // DELETE
    drawSpecialKey(
        kw * 6,
        y,
        kw,
        kh,
        "<",
        COLOR_RED
    );

    // 123
    drawSpecialKey(
        kw * 7,
        y,
        kw,
        kh,
        "123",
        COLOR_BLUE
    );

    // OK
    drawSpecialKey(
        kw * 8,
        y,
        kw * 2,
        kh,
        "OK",
        COLOR_GREEN
    );
}

// ============================================================
// DESSIN CLAVIER NUMERIQUE
// ============================================================

static void drawNumericKeyboard()
{
    int16_t kw = keyWidth();
    int16_t kh = keyHeight();

    // --------------------------------------------------------
    // LIGNES NUMERIQUES
    // --------------------------------------------------------

    for (
        uint8_t row = 0;
        row < 3;
        row++
    )
    {
        for (
            uint8_t col = 0;
            col < KEYBOARD_COLS_LOCAL;
            col++
        )
        {
            int16_t x =
                col * kw;

            int16_t y =
                KEYBOARD_TOP +
                row * kh;

            drawNormalKey(
                x,
                y,
                kw,
                kh,
                numericKeys[row][col]
            );
        }
    }

    // --------------------------------------------------------
    // DERNIERE LIGNE
    // --------------------------------------------------------

    int16_t y =
        KEYBOARD_TOP +
        3 * kh;

    // ABC
    drawSpecialKey(
        0,
        y,
        kw * 2,
        kh,
        "ABC",
        COLOR_BLUE
    );

    // ESPACE
    drawSpecialKey(
        kw * 2,
        y,
        kw * 4,
        kh,
        "ESPACE",
        COLOR_GREY
    );

    // DELETE
    drawSpecialKey(
        kw * 6,
        y,
        kw,
        kh,
        "<",
        COLOR_RED
    );

    // retour alpha
    drawSpecialKey(
        kw * 7,
        y,
        kw,
        kh,
        "ABC",
        COLOR_BLUE
    );

    // OK
    drawSpecialKey(
        kw * 8,
        y,
        kw * 2,
        kh,
        "OK",
        COLOR_GREEN
    );
}

// ============================================================
// DESSIN COMPLET
// ============================================================

void keyboardDraw()
{
    tft.fillScreen(
        COLOR_BLACK
    );

    drawInputArea();

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        drawAlphaKeyboard();
    }
    else
    {
        drawNumericKeyboard();
    }
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

    shiftActive = false;

    Serial.println(
        "[KEYBOARD] Init OK"
    );
}

// ============================================================
// DEMARRAGE
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

    Serial.println("\"");
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

    keyboardText += key;

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(
        key
    );

    Serial.print(
        "[KEYBOARD] TEXTE = \""
    );

    Serial.print(
        keyboardText
    );

    Serial.println("\"");

    drawInputArea();
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

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    // --------------------------------------------------------
    // PROTECTION
    // --------------------------------------------------------

    if (
        x < 0 ||
        y < 0 ||
        x >= SCREEN_WIDTH ||
        y >= SCREEN_HEIGHT
    )
    {
        return;
    }

    // --------------------------------------------------------
    // ZONE CLAVIER
    // --------------------------------------------------------

    if (
        y < KEYBOARD_TOP
    )
    {
        return;
    }

    int16_t kw =
        keyWidth();

    int16_t kh =
        keyHeight();

    uint8_t row =
        (y - KEYBOARD_TOP) / kh;

    uint8_t col =
        x / kw;

    if (
        row >= KEYBOARD_ROWS_LOCAL ||
        col >= KEYBOARD_COLS_LOCAL
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

    // ========================================================
    // DERNIERE LIGNE
    // ========================================================

    if (
        row == 3
    )
    {
        // ----------------------------------------------------
        // ALPHA
        // ----------------------------------------------------

        if (
            currentMode ==
            KEYBOARD_ALPHA
        )
        {
            // MAJ
            if (
                col < 2
            )
            {
                shiftActive =
                    !shiftActive;

                Serial.println(
                    "[KEYBOARD] MAJ"
                );

                keyboardDraw();

                return;
            }

            // ESPACE
            if (
                col >= 2 &&
                col < 6
            )
            {
                addCharacter(" ");

                return;
            }

            // DELETE
            if (
                col == 6
            )
            {
                keyboardDelete();

                return;
            }

            // 123
            if (
                col == 7
            )
            {
                currentMode =
                    KEYBOARD_NUMERIC;

                Serial.println(
                    "[KEYBOARD] MODE NUMERIQUE"
                );

                keyboardDraw();

                return;
            }

            // OK
            if (
                col >= 8
            )
            {
                keyboardValidate();

                return;
            }
        }

        // ----------------------------------------------------
        // NUMERIQUE
        // ----------------------------------------------------

        else
        {
            // ABC
            if (
                col < 2
            )
            {
                currentMode =
                    KEYBOARD_ALPHA;

                Serial.println(
                    "[KEYBOARD] MODE AZERTY"
                );

                keyboardDraw();

                return;
            }

            // ESPACE
            if (
                col >= 2 &&
                col < 6
            )
            {
                addCharacter(" ");

                return;
            }

            // DELETE
            if (
                col == 6
            )
            {
                keyboardDelete();

                return;
            }

            // ABC
            if (
                col == 7
            )
            {
                currentMode =
                    KEYBOARD_ALPHA;

                keyboardDraw();

                return;
            }

            // OK
            if (
                col >= 8
            )
            {
                keyboardValidate();

                return;
            }
        }

        return;
    }

    // ========================================================
    // TOUCHES ALPHA / NUMERIQUES
    // ========================================================

    const char* key = nullptr;

    if (
        currentMode ==
        KEYBOARD_ALPHA
    )
    {
        key =
            alphaKeys[row][col];
    }
    else
    {
        key =
            numericKeys[row][col];
    }

    if (
        key == nullptr ||
        strlen(key) == 0
    )
    {
        return;
    }

    // --------------------------------------------------------
    // MAJUSCULE / MINUSCULE
    // --------------------------------------------------------

    if (
        currentMode ==
        KEYBOARD_ALPHA &&
        shiftActive
    )
    {
        // Les lettres de notre tableau sont déjà
        // en majuscules.
        addCharacter(key);

        shiftActive = false;

        keyboardDraw();

        return;
    }

    addCharacter(key);
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