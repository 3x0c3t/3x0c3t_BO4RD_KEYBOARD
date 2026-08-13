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
// DIMENSIONS ECRAN
// ============================================================

static const int16_t SCREEN_W = 240;
static const int16_t SCREEN_H = 320;

// ============================================================
// ZONE SAISIE
// ============================================================

static const int16_t INPUT_Y = 0;
static const int16_t INPUT_H = 48;

// ============================================================
// BOUTONS DROITE
// ============================================================

static const int16_t ACTION_X = 168;
static const int16_t ACTION_W = 72;

static const int16_t DELETE_X = 168;
static const int16_t DELETE_W = 36;

static const int16_t OK_X = 204;
static const int16_t OK_W = 36;

// ============================================================
// CLAVIER
// ============================================================

static const int16_t KEYBOARD_Y = 138;

static const int16_t KEY_ROWS = 4;
static const int16_t KEY_COLS = 10;

static const int16_t KEY_HEIGHT =
    (SCREEN_H - KEYBOARD_Y) / KEY_ROWS;

static const int16_t KEY_WIDTH =
    SCREEN_W / KEY_COLS;

// ============================================================
// TOUCHES ALPHABETIQUES
// ============================================================

static const char alphaRows[4][11] =
{
    "QWERTYUIOP",
    "ASDFGHJKL",
    "ZXCVBNM",
    "1234567890"
};

// ============================================================
// TOUCHES NUMERIQUES
// ============================================================
//
// Le mode numérique reste disponible via keyboardSetMode()
// pour conserver la compatibilité du module.
// L'interface principale n'affiche plus de bouton ABC.
// ============================================================

static const char numericRows[4][11] =
{
    "1234567890",
    "-+*/=()<>",
    ".,:;!?%#",
    "ABCDEFGHIJ"
};

// ============================================================
// CLEAR TEXTE
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
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
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
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
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
// DESSIN ZONE DE SAISIE
// ============================================================

static void drawInputArea()
{
    // --------------------------------------------------------
    // Fond
    // --------------------------------------------------------

    tft.fillRect(
        0,
        INPUT_Y,
        SCREEN_W,
        INPUT_H,
        TFT_DARKGREY
    );

    // --------------------------------------------------------
    // Cadre
    // --------------------------------------------------------

    tft.drawRect(
        0,
        INPUT_Y,
        SCREEN_W,
        INPUT_H,
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
        10,
        2
    );

    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

    tft.drawRect(
        DELETE_X,
        0,
        DELETE_W,
        INPUT_H,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_DARKGREY
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        "DEL",
        DELETE_X + DELETE_W / 2,
        INPUT_H / 2,
        1
    );

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    tft.drawRect(
        OK_X,
        0,
        OK_W,
        INPUT_H,
        TFT_WHITE
    );

    tft.drawString(
        "OK",
        OK_X + OK_W / 2,
        INPUT_H / 2,
        1
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
    // Zone de saisie
    // --------------------------------------------------------

    drawInputArea();

    // --------------------------------------------------------
    // Séparation
    // --------------------------------------------------------

    tft.drawFastHLine(
        0,
        KEYBOARD_Y - 1,
        SCREEN_W,
        TFT_WHITE
    );

    // --------------------------------------------------------
    // Choix du clavier
    // --------------------------------------------------------

    const char (*rows)[11];

    if (currentMode == KEYBOARD_ALPHA)
        rows = alphaRows;
    else
        rows = numericRows;

    // --------------------------------------------------------
    // Touches
    // --------------------------------------------------------

    for (int row = 0; row < KEY_ROWS; row++)
    {
        for (int col = 0; col < KEY_COLS; col++)
        {
            int16_t x =
                col * KEY_WIDTH;

            int16_t y =
                KEYBOARD_Y +
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
        x >= SCREEN_W ||
        y < 0 ||
        y >= SCREEN_H
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
    // ZONE SAISIE
    // ========================================================

    if (
        y >= INPUT_Y &&
        y < INPUT_H
    )
    {
        // ----------------------------------------------------
        // BOUTON DEL
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                DELETE_X,
                0,
                DELETE_W,
                INPUT_H
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
        // BOUTON OK
        // ----------------------------------------------------

        if (
            touchInside(
                x,
                y,
                OK_X,
                0,
                OK_W,
                INPUT_H
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

    // ========================================================
    // ZONE VIDE ENTRE SAISIE ET CLAVIER
    // ========================================================

    if (y < KEYBOARD_Y)
    {
        return;
    }

    // ========================================================
    // CALCUL LIGNE
    // ========================================================

    int row =
        (y - KEYBOARD_Y) /
        KEY_HEIGHT;

    // --------------------------------------------------------
    // Protection dernière ligne
    // --------------------------------------------------------

    if (row >= KEY_ROWS)
        row = KEY_ROWS - 1;

    // ========================================================
    // CALCUL COLONNE
    // ========================================================

    int col =
        x / KEY_WIDTH;

    // --------------------------------------------------------
    // Protection bord droit
    // --------------------------------------------------------

    if (col >= KEY_COLS)
        col = KEY_COLS - 1;

    // --------------------------------------------------------
    // Protection
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

    const char (*rows)[11];

    if (currentMode == KEYBOARD_ALPHA)
        rows = alphaRows;
    else
        rows = numericRows;

    char c =
        rows[row][col];

    if (c == '\0')
        return;

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