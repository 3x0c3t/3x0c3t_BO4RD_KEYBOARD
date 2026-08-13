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


// ============================================================
// ANTI-REBOND
// ============================================================

static unsigned long lastTouchTime = 0;

static const unsigned long TOUCH_DELAY = 180;


// ============================================================
// DIMENSIONS CLAVIER
// ============================================================

static const int16_t KEY_WIDTH =
    KEYBOARD_W / KEYBOARD_COLS;

static const int16_t KEY_HEIGHT =
    KEYBOARD_H / KEYBOARD_ROWS;


// ============================================================
// TOUCHES ALPHABETIQUES
// ============================================================

static const char alphaRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "QWERTYUIOP",
    "ASDFGHJKL ",
    "ZXCVBNM   ",
    "1234567890"
};


// ============================================================
// TOUCHES NUMERIQUES
// ============================================================

static const char numericRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "1234567890",
    "-+*/=()<> ",
    ".,:;!?%#  ",
    "ABCDEFGHIJ"
};


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
// EFFACER TEXTE
// ============================================================

static void clearText()
{
    keyboardTextLength = 0;
    keyboardText[0] = '\0';
}


// ============================================================
// AJOUT CARACTERE
// ============================================================

static void addCharacter(char c)
{
    if (keyboardTextLength >= KEYBOARD_MAX_TEXT - 1)
        return;

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
// DESSIN BOUTON
// ============================================================

static void drawButton(
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
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_DARK_GREY
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        label,
        x + (w / 2),
        y + (h / 2),
        2
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// DESSIN TOUCHE CLAVIER
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
        COLOR_WHITE
    );

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        label,
        x + (w / 2),
        y + (h / 2),
        2
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// DESSIN CHAMP DE SAISIE
// ============================================================

static void drawInputArea()
{
    // --------------------------------------------------------
    // Fond
    // --------------------------------------------------------

    tft.fillRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_DARK_GREY
    );


    // --------------------------------------------------------
    // Cadre général
    // --------------------------------------------------------

    tft.drawRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        COLOR_WHITE
    );


    // --------------------------------------------------------
    // Zone texte
    // --------------------------------------------------------

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_DARK_GREY
    );

    tft.setTextDatum(TL_DATUM);

    tft.drawString(
        keyboardText,
        INPUT_X + 6,
        INPUT_Y + 10,
        2
    );


    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

    drawButton(
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H,
        "DEL"
    );


    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    drawButton(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        "OK"
    );
}


// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    // --------------------------------------------------------
    // Fond
    // --------------------------------------------------------

    tft.fillScreen(COLOR_BLACK);


    // --------------------------------------------------------
    // Champ de saisie
    // --------------------------------------------------------

    drawInputArea();


    // --------------------------------------------------------
    // Ligne de séparation
    // --------------------------------------------------------

    tft.drawFastHLine(
        KEYBOARD_X,
        KEYBOARD_Y - 1,
        KEYBOARD_W,
        COLOR_WHITE
    );


    // --------------------------------------------------------
    // Choix du clavier
    // --------------------------------------------------------

    const char (*rows)[KEYBOARD_COLS + 1];

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

    for (int row = 0; row < KEYBOARD_ROWS; row++)
    {
        for (int col = 0; col < KEYBOARD_COLS; col++)
        {
            int16_t x =
                KEYBOARD_X +
                col * KEY_WIDTH;

            int16_t y =
                KEYBOARD_Y +
                row * KEY_HEIGHT;


            char label[2];

            label[0] = rows[row][col];
            label[1] = '\0';


            // ------------------------------------------------
            // Touche vide
            // ------------------------------------------------

            if (label[0] == ' ')
                continue;


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
// GESTION TACTILE
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
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
    // Coordonnées invalides
    // --------------------------------------------------------

    if (
        x < 0 ||
        x >= SCREEN_WIDTH ||
        y < 0 ||
        y >= SCREEN_HEIGHT
    )
    {
        Serial.println(
            "[KEYBOARD] Coordonnees invalides"
        );

        return;
    }


    // --------------------------------------------------------
    // Anti-rebond
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
    // CHAMP DE SAISIE
    // ========================================================

    if (
        x >= INPUT_X &&
        x < INPUT_X + INPUT_W &&
        y >= INPUT_Y &&
        y < INPUT_Y + INPUT_H
    )
    {
        // ----------------------------------------------------
        // DEL
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                BTN_DEL_X,
                BTN_DEL_Y,
                BTN_DEL_W,
                BTN_DEL_H
            )
        )
        {
            Serial.println(
                "[KEYBOARD] DEL"
            );

            backspaceCharacter();

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

            return;
        }


        // ----------------------------------------------------
        // OK
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                BTN_OK_X,
                BTN_OK_Y,
                BTN_OK_W,
                BTN_OK_H
            )
        )
        {
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


        return;
    }


    // ========================================================
    // ZONE ENTRE CHAMP ET CLAVIER
    // ========================================================

    if (
        y < KEYBOARD_Y
    )
    {
        return;
    }


    // ========================================================
    // CALCUL LIGNE
    // ========================================================

    int row =
        (y - KEYBOARD_Y) /
        KEY_HEIGHT;


    if (row < 0)
        return;

    if (row >= KEYBOARD_ROWS)
        row = KEYBOARD_ROWS - 1;


    // ========================================================
    // CALCUL COLONNE
    // ========================================================

    int col =
        (x - KEYBOARD_X) /
        KEY_WIDTH;


    if (col < 0)
        return;

    if (col >= KEYBOARD_COLS)
        col = KEYBOARD_COLS - 1;


    // ========================================================
    // DEBUG ZONE
    // ========================================================

    Serial.print(
        "[KEYBOARD] ROW="
    );

    Serial.print(row);

    Serial.print(
        " COL="
    );

    Serial.println(col);


    // ========================================================
    // CLAVIER ACTIF
    // ========================================================

    const char (*rows)[KEYBOARD_COLS + 1];

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


    // --------------------------------------------------------
    // Touche vide
    // --------------------------------------------------------

    if (
        c == '\0' ||
        c == ' '
    )
    {
        return;
    }


    // ========================================================
    // AJOUT
    // ========================================================

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(c);


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


    // ========================================================
    // REDESSIN
    // ========================================================

    Serial.println(
        "[KEYBOARD] Dessin clavier"
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
// LECTURE TEXTE
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
// EFFACER VALIDATION
// ============================================================

void keyboardClearValidated()
{
    validated = false;
}