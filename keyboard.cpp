#include "keyboard.h"
#include "config.h"

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================================
// État clavier
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static char inputText[KEYBOARD_MAX_TEXT + 1];
static uint8_t inputLength = 0;

static bool keyboardInitialized = false;

// ============================================================
// Claviers
// ============================================================

static const char alphaRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "QWERTYUIOP",
    "ASDFGHJKL ",
    "ZXCVBNM  ",
    "1234567890"
};

static const char numericRows[KEYBOARD_ROWS][KEYBOARD_COLS + 1] =
{
    "1234567890",
    "0987654321",
    "  -_.,  ",
    "ABCDEFGHIJ"
};

// ============================================================
// Utilitaires
// ============================================================

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_H / KEYBOARD_ROWS;
}

static void addCharacter(char c)
{
    if (c == ' ')
        return;

    if (inputLength >= KEYBOARD_MAX_TEXT)
        return;

    inputText[inputLength] = c;
    inputLength++;

    inputText[inputLength] = '\0';

    Serial.print("[KEYBOARD] TEXTE = \"");
    Serial.print(inputText);
    Serial.println("\"");
}

static void drawCenteredText(
    const char* text,
    int16_t x,
    int16_t y,
    uint8_t font,
    uint16_t color
)
{
    tft.setTextColor(color, COLOR_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(text, x, y, font);
    tft.setTextDatum(TL_DATUM);
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

    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.setTextDatum(ML_DATUM);

    tft.drawString(
        inputText,
        INPUT_X + 5,
        INPUT_Y + INPUT_H / 2,
        2
    );

    tft.setTextDatum(TL_DATUM);
}

// ============================================================
// Bouton DEL
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

    drawCenteredText(
        "DEL",
        BTN_DEL_X + BTN_DEL_W / 2,
        BTN_DEL_Y + BTN_DEL_H / 2,
        2,
        COLOR_WHITE
    );
}

// ============================================================
// Bouton OK
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

    drawCenteredText(
        "OK",
        BTN_OK_X + BTN_OK_W / 2,
        BTN_OK_Y + BTN_OK_H / 2,
        2,
        COLOR_BLACK
    );
}

// ============================================================
// Clavier
// ============================================================

void keyboardDraw()
{
    tft.fillRect(
        KEYBOARD_X,
        KEYBOARD_Y,
        KEYBOARD_W,
        KEYBOARD_H,
        COLOR_BLACK
    );

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
            int16_t x = KEYBOARD_X + col * kw;
            int16_t y = KEYBOARD_Y + row * kh;

            tft.drawRect(
                x,
                y,
                kw,
                kh,
                COLOR_GREY
            );

            char key = rows[row][col];

            if (key != ' ')
            {
                char text[2];
                text[0] = key;
                text[1] = '\0';

                drawCenteredText(
                    text,
                    x + kw / 2,
                    y + kh / 2,
                    2,
                    COLOR_WHITE
                );
            }
        }
    }

    drawInputArea();
    drawDeleteButton();
    drawOkButton();
}

// ============================================================
// Initialisation
// ============================================================

void keyboardInit()
{
    if (keyboardInitialized)
    {
        Serial.println("[KEYBOARD] Init deja effectue");
        return;
    }

    Serial.println("[KEYBOARD] Init");

    inputLength = 0;
    inputText[0] = '\0';

    currentMode = KEYBOARD_ALPHA;

    keyboardInitialized = true;

    keyboardDraw();

    Serial.println("[KEYBOARD] OK");
}

// ============================================================
// Mise à jour tactile
// ============================================================

void keyboardUpdate(int16_t x, int16_t y)
{
    if (!keyboardInitialized)
        return;

    Serial.print("[KEYBOARD] Analyse X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);

    // --------------------------------------------------------
    // DEL
    // --------------------------------------------------------

    if (
        x >= BTN_DEL_X &&
        x < BTN_DEL_X + BTN_DEL_W &&
        y >= BTN_DEL_Y &&
        y < BTN_DEL_Y + BTN_DEL_H
    )
    {
        Serial.println("[KEYBOARD] ZONE = DEL");

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
        Serial.println("[KEYBOARD] ZONE = OK");

        keyboardValidate();

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

    int col =
        (x - KEYBOARD_X) /
        keyWidth();

    int row =
        (y - KEYBOARD_Y) /
        keyHeight();

    if (row < 0 || row >= KEYBOARD_ROWS)
        return;

    if (col < 0 || col >= KEYBOARD_COLS)
        return;

    Serial.print("[KEYBOARD] ROW=");
    Serial.print(row);
    Serial.print(" COL=");
    Serial.println(col);

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

    addCharacter(key);

    keyboardDraw();
}

// ============================================================
// Effacement
// ============================================================

void keyboardDelete()
{
    if (inputLength == 0)
        return;

    inputLength--;

    inputText[inputLength] = '\0';

    Serial.print("[KEYBOARD] DEL -> TEXTE = \"");
    Serial.print(inputText);
    Serial.println("\"");

    keyboardDraw();
}

// ============================================================
// Validation
// ============================================================

void keyboardValidate()
{
    Serial.print("[KEYBOARD] Validation : ");
    Serial.println(inputText);

    Serial.print("[KEYBOARD] Texte valide : ");
    Serial.println(inputText);
}

// ============================================================
// Effacer complètement
// ============================================================

void keyboardClear()
{
    inputLength = 0;
    inputText[0] = '\0';

    keyboardDraw();

    Serial.println("[KEYBOARD] Texte efface");
}

// ============================================================
// Mode
// ============================================================

void keyboardSetMode(KeyboardMode mode)
{
    currentMode = mode;

    keyboardDraw();
}

KeyboardMode keyboardGetMode()
{
    return currentMode;
}

// ============================================================
// Texte
// ============================================================

const char* keyboardGetText()
{
    return inputText;
}