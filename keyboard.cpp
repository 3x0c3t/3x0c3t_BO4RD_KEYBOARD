#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <string.h>

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// keyboard.cpp
// ============================================================

extern TFT_eSPI tft;


// ============================================================
// MODE CLAVIER
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;


// ============================================================
// TEXTE
// ============================================================

static char keyboardText[KEYBOARD_MAX_TEXT + 1] = "";


// ============================================================
// CLAVIER
// ============================================================

static const char alphaRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "ABCDEFGHIJ",
    "KLMNOPQRST",
    "UVWXYZ   ",
    "1234567890"
};

static const char numericRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "1234567890",
    "0987654321",
    "   .-_   ",
    "1234567890"
};


// ============================================================
// DIMENSIONS AUTOMATIQUES DES TOUCHES
// ============================================================

static const int16_t keyWidth =
    KEYBOARD_W / KEYBOARD_COLS;

static const int16_t keyHeight =
    KEYBOARD_H / KEYBOARD_ROWS;


// ============================================================
// OUTILS
// ============================================================

static bool pointInRect(
    int16_t x,
    int16_t y,
    int16_t rx,
    int16_t ry,
    int16_t rw,
    int16_t rh
)
{
    return
        x >= rx &&
        x < (rx + rw) &&
        y >= ry &&
        y < (ry + rh);
}


// ============================================================
// DESSIN DU CHAMP DE SAISIE
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

    tft.setTextColor(
        COLOR_WHITE,
        COLOR_BLACK
    );

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        keyboardText,
        INPUT_X + INPUT_W / 2,
        INPUT_Y + INPUT_H / 2,
        2
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// DESSIN BOUTON DEL
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

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        "DEL",
        BTN_DEL_X + BTN_DEL_W / 2,
        BTN_DEL_Y + BTN_DEL_H / 2,
        2
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// DESSIN BOUTON OK
// ============================================================

static void drawOkButton()
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

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        "OK",
        BTN_OK_X + BTN_OK_W / 2,
        BTN_OK_Y + BTN_OK_H / 2,
        2
    );

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// AJOUTER UN CARACTERE
// ============================================================

static void addCharacter(char c)
{
    if (c == ' ')
        return;

    size_t len = strlen(keyboardText);

    if (len >= KEYBOARD_MAX_TEXT)
        return;

    keyboardText[len] = c;
    keyboardText[len + 1] = '\0';

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");
}


// ============================================================
// SUPPRIMER LE DERNIER CARACTERE
// ============================================================

static void deleteCharacter()
{
    size_t len = strlen(keyboardText);

    if (len == 0)
        return;

    keyboardText[len - 1] = '\0';

    Serial.print("[KEYBOARD] DEL -> TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");

    drawInputArea();
}


// ============================================================
// VALIDATION
// ============================================================

static void validateText()
{
    Serial.print("[KEYBOARD] Validation : ");
    Serial.println(keyboardText);

    Serial.print("[KEYBOARD] Texte valide : ");
    Serial.println(keyboardText);
}


// ============================================================
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    Serial.println("[KEYBOARD] Dessin clavier");

    // --------------------------------------------------------
    // Zone complète du clavier
    // --------------------------------------------------------

    tft.fillRect(
        KEYBOARD_X,
        KEYBOARD_Y,
        KEYBOARD_W,
        KEYBOARD_H,
        COLOR_BLACK
    );

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
                col * keyWidth;

            int16_t y =
                KEYBOARD_Y +
                row * keyHeight;

            tft.drawRect(
                x,
                y,
                keyWidth,
                keyHeight,
                COLOR_WHITE
            );

            char key = rows[row][col];

            if (key == ' ')
                continue;

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_BLACK
            );

            tft.setTextDatum(MC_DATUM);

            char text[2];

            text[0] = key;
            text[1] = '\0';

            tft.drawString(
                text,
                x + keyWidth / 2,
                y + keyHeight / 2,
                2
            );
        }
    }

    tft.setTextDatum(TL_DATUM);
}


// ============================================================
// DESSIN COMPLET
// ============================================================

void keyboardDrawAll()
{
    drawInputArea();

    drawDeleteButton();

    drawOkButton();

    keyboardDraw();
}


// ============================================================
// INITIALISATION
// ============================================================

void keyboardInit()
{
    Serial.println("[KEYBOARD] Init");

    currentMode = KEYBOARD_ALPHA;

    keyboardText[0] = '\0';

    keyboardDrawAll();

    Serial.println("[KEYBOARD] OK");
}


// ============================================================
// COMPATIBILITE
// ============================================================

void keyboardBegin()
{
    keyboardInit();
}


// ============================================================
// TRAITEMENT D'UNE TOUCHE DU CLAVIER
// ============================================================

static void processKeyboardKey(
    int16_t x,
    int16_t y
)
{
    Serial.print("[KEYBOARD] Analyse X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);


    // --------------------------------------------------------
    // Protection contre les coordonnées hors écran
    // --------------------------------------------------------

    if (x < 0 || x >= SCREEN_WIDTH ||
        y < 0 || y >= SCREEN_HEIGHT)
    {
        return;
    }


    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

    if (pointInRect(
        x,
        y,
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H
    ))
    {
        Serial.println("[KEYBOARD] ZONE = DEL");

        deleteCharacter();

        return;
    }


    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    if (pointInRect(
        x,
        y,
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H
    ))
    {
        Serial.println("[KEYBOARD] ZONE = OK");

        validateText();

        return;
    }


    // --------------------------------------------------------
    // Clavier
    // --------------------------------------------------------

    if (!pointInRect(
        x,
        y,
        KEYBOARD_X,
        KEYBOARD_Y,
        KEYBOARD_W,
        KEYBOARD_H
    ))
    {
        return;
    }


    // --------------------------------------------------------
    // Calcul ligne / colonne
    // --------------------------------------------------------

    int16_t col =
        (x - KEYBOARD_X) / keyWidth;

    int16_t row =
        (y - KEYBOARD_Y) / keyHeight;


    if (row < 0)
        return;

    if (row >= KEYBOARD_ROWS)
        return;

    if (col < 0)
        return;

    if (col >= KEYBOARD_COLS)
        return;


    Serial.print("[KEYBOARD] ROW=");
    Serial.print(row);

    Serial.print(" COL=");
    Serial.println(col);


    // --------------------------------------------------------
    // Récupération caractère
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

    char key = rows[row][col];


    // --------------------------------------------------------
    // Touche vide
    // --------------------------------------------------------

    if (key == ' ')
    {
        Serial.println("[KEYBOARD] TOUCHE VIDE");
        return;
    }


    // --------------------------------------------------------
    // Caractère
    // --------------------------------------------------------

    Serial.print("[KEYBOARD] TOUCHE = ");
    Serial.println(key);

    addCharacter(key);

    drawInputArea();
}


// ============================================================
// MISE A JOUR TACTILE
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print("[KEYBOARD TOUCH] X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);

    processKeyboardKey(x, y);
}


// ============================================================
// CHANGER DE MODE
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
)
{
    currentMode = mode;

    Serial.print("[KEYBOARD] Mode = ");

    if (currentMode == KEYBOARD_ALPHA)
    {
        Serial.println("ALPHA");
    }
    else
    {
        Serial.println("NUMERIC");
    }

    keyboardDraw();
}


// ============================================================
// LIRE LE MODE
// ============================================================

KeyboardMode keyboardGetMode()
{
    return currentMode;
}


// ============================================================
// LIRE LE TEXTE
// ============================================================

const char* keyboardGetText()
{
    return keyboardText;
}


// ============================================================
// EFFACER LE TEXTE
// ============================================================

void keyboardClearText()
{
    keyboardText[0] = '\0';

    Serial.println("[KEYBOARD] Texte efface");

    drawInputArea();
}