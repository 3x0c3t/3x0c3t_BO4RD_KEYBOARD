#include "keyboard.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"

// ============================================================
// TFT
// ============================================================

extern TFT_eSPI tft;

// ============================================================
// ETAT INTERNE
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
// DIMENSIONS
// ============================================================

static const int16_t HEADER_HEIGHT = 42;

static const int16_t KEY_ROWS = 4;
static const int16_t KEY_COLS = 10;

static const int16_t KEYBOARD_BOTTOM = SCREEN_HEIGHT;

static const int16_t KEY_AREA_HEIGHT =
    KEYBOARD_BOTTOM - (KEYBOARD_Y + HEADER_HEIGHT);

static const int16_t KEY_HEIGHT =
    KEY_AREA_HEIGHT / KEY_ROWS;

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
// EFFACER TEXTE
// ============================================================

static void clearText()
{
    keyboardTextLength = 0;

    keyboardText[0] = '\0';
}

// ============================================================
// AJOUTER CARACTERE
// ============================================================

static void addCharacter(char c)
{
    if (keyboardTextLength >= KEYBOARD_MAX_TEXT - 1)
    {
        Serial.println("[KEYBOARD] Texte plein");
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
// DESSIN D'UNE TOUCHE
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
// DESSIN CLAVIER
// ============================================================

void keyboardDraw()
{
    // --------------------------------------------------------
    // FOND
    // --------------------------------------------------------

    tft.fillScreen(TFT_BLACK);

    // --------------------------------------------------------
    // ZONE TEXTE
    // --------------------------------------------------------

    tft.fillRect(
        0,
        0,
        SCREEN_WIDTH,
        HEADER_HEIGHT,
        TFT_DARKGREY
    );

    tft.drawRect(
        0,
        0,
        SCREEN_WIDTH,
        HEADER_HEIGHT,
        TFT_WHITE
    );

    // --------------------------------------------------------
    // TEXTE SAISI
    // --------------------------------------------------------

    tft.setTextColor(
        TFT_WHITE,
        TFT_DARKGREY
    );

    tft.setTextDatum(TL_DATUM);

    tft.drawString(
        keyboardText,
        6,
        8,
        2
    );

    // --------------------------------------------------------
    // ABC / 123
    // --------------------------------------------------------

    tft.setTextDatum(MC_DATUM);

    tft.drawString(
        currentMode == KEYBOARD_ALPHA
            ? "ABC"
            : "123",
        25,
        HEADER_HEIGHT - 10,
        1
    );

    // --------------------------------------------------------
    // BACKSPACE
    // --------------------------------------------------------

    tft.drawString(
        "<",
        190,
        HEADER_HEIGHT - 10,
        2
    );

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    tft.drawString(
        "OK",
        225,
        HEADER_HEIGHT - 10,
        1
    );

    tft.setTextDatum(TL_DATUM);

    // --------------------------------------------------------
    // SELECTION CLAVIER
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

    // --------------------------------------------------------
    // TOUCHES
    // --------------------------------------------------------

    for (int row = 0; row < KEY_ROWS; row++)
    {
        for (int col = 0; col < KEY_COLS; col++)
        {
            // ------------------------------------------------
            // Calcul exact des limites
            // ------------------------------------------------

            int16_t x1 =
                (col * SCREEN_WIDTH) / KEY_COLS;

            int16_t x2 =
                ((col + 1) * SCREEN_WIDTH) / KEY_COLS;

            int16_t y1 =
                KEYBOARD_Y +
                HEADER_HEIGHT +
                ((row * KEY_AREA_HEIGHT) / KEY_ROWS);

            int16_t y2 =
                KEYBOARD_Y +
                HEADER_HEIGHT +
                (((row + 1) * KEY_AREA_HEIGHT) / KEY_ROWS);

            int16_t w =
                x2 - x1;

            int16_t h =
                y2 - y1;

            char label[2];

            label[0] = rows[row][col];
            label[1] = '\0';

            drawKey(
                x1,
                y1,
                w,
                h,
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

    Serial.println("[KEYBOARD] Dessin clavier");

    keyboardDraw();

    Serial.println("[KEYBOARD] Pret");
}

// ============================================================
// GESTION TACTILE
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
    // Vérification coordonnées
    // --------------------------------------------------------

    if (x < 0 ||
        x >= SCREEN_WIDTH ||
        y < 0 ||
        y >= SCREEN_HEIGHT)
    {
        Serial.println(
            "[KEYBOARD] Coordonnees hors ecran"
        );

        return;
    }

    // --------------------------------------------------------
    // ANTI-REBOND
    // --------------------------------------------------------

    unsigned long now = millis();

    if (now - lastTouchTime < TOUCH_DELAY)
    {
        return;
    }

    lastTouchTime = now;

    // ========================================================
    // HEADER
    // ========================================================

    if (y < HEADER_HEIGHT)
    {
        // ----------------------------------------------------
        // ABC / 123
        // ----------------------------------------------------

        if (touchInside(
                x,
                y,
                0,
                0,
                55,
                HEADER_HEIGHT))
        {
            if (currentMode == KEYBOARD_ALPHA)
            {
                currentMode = KEYBOARD_NUMERIC;

                Serial.println(
                    "[KEYBOARD] Mode NUMERIC"
                );
            }
            else
            {
                currentMode = KEYBOARD_ALPHA;

                Serial.println(
                    "[KEYBOARD] Mode ALPHA"
                );
            }

            keyboardDraw();

            return;
        }

        // ----------------------------------------------------
        // BACKSPACE
        // ----------------------------------------------------

        if (touchInside(
                x,
                y,
                165,
                0,
                40,
                HEADER_HEIGHT))
        {
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

        if (touchInside(
                x,
                y,
                205,
                0,
                SCREEN_WIDTH - 205,
                HEADER_HEIGHT))
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
    // ZONE CLAVIER
    // ========================================================

    const int16_t keyboardTop =
        KEYBOARD_Y + HEADER_HEIGHT;

    if (y < keyboardTop)
    {
        return;
    }

    if (y >= SCREEN_HEIGHT)
    {
        return;
    }

    // --------------------------------------------------------
    // CALCUL LIGNE
    // --------------------------------------------------------

    int row =
        ((y - keyboardTop) * KEY_ROWS) /
        KEY_AREA_HEIGHT;

    // --------------------------------------------------------
    // PROTECTION
    // --------------------------------------------------------

    if (row < 0)
    {
        row = 0;
    }

    if (row >= KEY_ROWS)
    {
        row = KEY_ROWS - 1;
    }

    // --------------------------------------------------------
    // CALCUL COLONNE
    //
    // IMPORTANT :
    // On utilise les limites exactes de l'écran.
    //
    // x=0     -> COL 0
    // x=23    -> COL 0
    // x=24    -> COL 1
    // ...
    // x=215   -> COL 8
    // x=216   -> COL 9
    // x=239   -> COL 9
    //
    // Le bord droit est donc inclus correctement.
    // --------------------------------------------------------

    int col =
        (x * KEY_COLS) /
        SCREEN_WIDTH;

    // --------------------------------------------------------
    // PROTECTION COLONNE
    // --------------------------------------------------------

    if (col < 0)
    {
        col = 0;
    }

    if (col >= KEY_COLS)
    {
        col = KEY_COLS - 1;
    }

    Serial.print("[KEYBOARD] ROW=");
    Serial.print(row);

    Serial.print(" COL=");
    Serial.println(col);

    // --------------------------------------------------------
    // CLAVIER ACTIF
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

    // --------------------------------------------------------
    // CARACTERE
    // --------------------------------------------------------

    char c =
        rows[row][col];

    if (c == '\0')
    {
        return;
    }

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(c);

    // --------------------------------------------------------
    // AJOUT
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // REDESSIN
    // --------------------------------------------------------

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
// EFFACEMENT VALIDATION
// ============================================================

void keyboardClearValidated()
{
    validated = false;
}