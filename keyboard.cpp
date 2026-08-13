#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// État du clavier
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static char keyboardText[KEYBOARD_MAX_TEXT + 1] = "";
static uint8_t keyboardTextLength = 0;

// ============================================================
// Prototypes internes
// ============================================================

static int16_t keyWidth();
static int16_t keyHeight();

static bool pointInRect(
    int16_t x,
    int16_t y,
    int16_t rx,
    int16_t ry,
    int16_t rw,
    int16_t rh
);

static void drawInputArea();
static void drawKeyboardButtons();

static void keyboardAddChar(char c);
static void keyboardProcessKey(int row, int col);

// ============================================================
// Dimensions des touches
// ============================================================

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_H / KEYBOARD_ROWS;
}

// ============================================================
// Test zone rectangulaire
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
        x < rx + rw &&
        y >= ry &&
        y < ry + rh;
}

// ============================================================
// Clavier alphabetique
// ============================================================

static const char alphaRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "QWERTYUIOP",
    "ASDFGHJKL ",
    "ZXCVBNM   ",
    "1234567890"
};

// ============================================================
// Clavier numerique
// ============================================================

static const char numericRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "1234567890",
    "0987654321",
    "1234567890",
    "0987654321"
};

// ============================================================
// Initialisation
// ============================================================

void keyboardInit()
{
    Serial.println("[KEYBOARD] Init");

    keyboardText[0] = '\0';
    keyboardTextLength = 0;

    currentMode = KEYBOARD_ALPHA;

    keyboardDraw();

    Serial.println("[KEYBOARD] Init OK");
}

void keyboardBegin()
{
    keyboardInit();
}

// ============================================================
// Gestion texte
// ============================================================

void keyboardClear()
{
    keyboardTextLength = 0;
    keyboardText[0] = '\0';

    drawInputArea();
}

const char* keyboardGetText()
{
    return keyboardText;
}

void keyboardSetText(const char* text)
{
    if (text == nullptr)
    {
        keyboardClear();
        return;
    }

    strncpy(
        keyboardText,
        text,
        KEYBOARD_MAX_TEXT
    );

    keyboardText[KEYBOARD_MAX_TEXT] = '\0';

    keyboardTextLength = strlen(keyboardText);

    drawInputArea();
}

bool keyboardHasText()
{
    return keyboardTextLength > 0;
}

// ============================================================
// Ajout caractère
// ============================================================

static void keyboardAddChar(char c)
{
    if (c == ' ')
        return;

    if (keyboardTextLength >= KEYBOARD_MAX_TEXT)
        return;

    keyboardText[keyboardTextLength] = c;
    keyboardTextLength++;

    keyboardText[keyboardTextLength] = '\0';

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");

    drawInputArea();
}

// ============================================================
// DEL
// ============================================================

void keyboardDelete()
{
    Serial.println("[KEYBOARD] DEL");

    if (keyboardTextLength > 0)
    {
        keyboardTextLength--;

        keyboardText[keyboardTextLength] = '\0';
    }

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(keyboardText);
    Serial.println("\"");

    drawInputArea();
}

// ============================================================
// OK
// ============================================================

void keyboardValidate()
{
    Serial.print("[KEYBOARD] Validation : ");
    Serial.println(keyboardText);

    if (keyboardTextLength == 0)
    {
        Serial.println("[KEYBOARD] Texte vide");
        return;
    }

    Serial.print("[KEYBOARD] Texte valide : ");
    Serial.println(keyboardText);
}

// ============================================================
// Champ de saisie
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
}

// ============================================================
// Bouton DEL et OK
// ============================================================

static void drawKeyboardButtons()
{
    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

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

    tft.drawString(
        "OK",
        BTN_OK_X + BTN_OK_W / 2,
        BTN_OK_Y + BTN_OK_H / 2,
        2
    );
}

// ============================================================
// Dessin clavier
// ============================================================

void keyboardDraw()
{
    Serial.println("[KEYBOARD] Dessin clavier");

    tft.fillScreen(COLOR_BLACK);

    drawInputArea();

    drawKeyboardButtons();

    const char (*rows)[KEYBOARD_COLS + 1];

    if (currentMode == KEYBOARD_ALPHA)
        rows = alphaRows;
    else
        rows = numericRows;

    const int16_t kw = keyWidth();
    const int16_t kh = keyHeight();

    for (int row = 0; row < KEYBOARD_ROWS; row++)
    {
        for (int col = 0; col < KEYBOARD_COLS; col++)
        {
            const int16_t x =
                KEYBOARD_X + col * kw;

            const int16_t y =
                KEYBOARD_Y + row * kh;

            tft.fillRect(
                x,
                y,
                kw,
                kh,
                COLOR_DARK_GREY
            );

            tft.drawRect(
                x,
                y,
                kw,
                kh,
                COLOR_WHITE
            );

            char key = rows[row][col];

            if (key == ' ')
                continue;

            char text[2];

            text[0] = key;
            text[1] = '\0';

            tft.setTextColor(
                COLOR_WHITE,
                COLOR_DARK_GREY
            );

            tft.setTextDatum(MC_DATUM);

            tft.drawString(
                text,
                x + kw / 2,
                y + kh / 2,
                2
            );
        }
    }
}

// ============================================================
// Traitement touche
// ============================================================

static void keyboardProcessKey(
    int row,
    int col
)
{
    if (row < 0 || row >= KEYBOARD_ROWS)
        return;

    if (col < 0 || col >= KEYBOARD_COLS)
        return;

    const char (*rows)[KEYBOARD_COLS + 1];

    if (currentMode == KEYBOARD_ALPHA)
        rows = alphaRows;
    else
        rows = numericRows;

    char key = rows[row][col];

    if (key == ' ')
        return;

    Serial.print("[KEYBOARD] TOUCHE = ");
    Serial.println(key);

    keyboardAddChar(key);
}

// ============================================================
// Gestion tactile
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
    // DEL
    // --------------------------------------------------------

    if (
        pointInRect(
            x,
            y,
            BTN_DEL_X,
            BTN_DEL_Y,
            BTN_DEL_W,
            BTN_DEL_H
        )
    )
    {
        Serial.println("[KEYBOARD] ZONE = DEL");

        keyboardDelete();

        delay(120);
        return;
    }

    // --------------------------------------------------------
    // OK
    // --------------------------------------------------------

    if (
        pointInRect(
            x,
            y,
            BTN_OK_X,
            BTN_OK_Y,
            BTN_OK_W,
            BTN_OK_H
        )
    )
    {
        Serial.println("[KEYBOARD] ZONE = OK");

        keyboardValidate();

        delay(120);
        return;
    }

    // --------------------------------------------------------
    // Clavier
    // --------------------------------------------------------

    if (
        x < KEYBOARD_X ||
        x >= KEYBOARD_X + KEYBOARD_W ||
        y < KEYBOARD_Y ||
        y >= KEYBOARD_Y + KEYBOARD_H
    )
    {
        return;
    }

    const int16_t kw = keyWidth();
    const int16_t kh = keyHeight();

    int row =
        (y - KEYBOARD_Y) / kh;

    int col =
        (x - KEYBOARD_X) / kw;

    if (row >= KEYBOARD_ROWS)
        row = KEYBOARD_ROWS - 1;

    if (col >= KEYBOARD_COLS)
        col = KEYBOARD_COLS - 1;

    Serial.print("[KEYBOARD] ROW=");
    Serial.print(row);
    Serial.print(" COL=");
    Serial.println(col);

    keyboardProcessKey(
        row,
        col
    );

    delay(120);
}

// ============================================================
// Mode
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