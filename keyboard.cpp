#include "keyboard.h"
#include "config.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ============================================================
// Ecran
// ============================================================

static Adafruit_ILI9341 tft(
    TFT_CS,
    TFT_DC,
    TFT_RST
);

// ============================================================
// Etat clavier
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static bool shiftEnabled = false;

static bool validated = false;

static char textBuffer[MAX_TEXT_LENGTH + 1];

static uint8_t textLength = 0;

// ============================================================
// Description d'une touche
// ============================================================

struct Key
{
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    const char* label;

    char character;

    uint8_t action;
};

// Actions
#define ACTION_CHARACTER  0
#define ACTION_SHIFT      1
#define ACTION_BACKSPACE  2
#define ACTION_SPACE      3
#define ACTION_CLEAR      4
#define ACTION_ENTER      5
#define ACTION_MODE_NUM   6
#define ACTION_MODE_ALPHA 7
#define ACTION_MODE_SYM   8

// ============================================================
// Dessin zone texte
// ============================================================

static void drawTextArea()
{
    tft.fillRoundRect(
        TEXT_AREA_X,
        TEXT_AREA_Y,
        TEXT_AREA_W,
        TEXT_AREA_H,
        KEY_RADIUS,
        COLOR_BLACK
    );

    tft.drawRoundRect(
        TEXT_AREA_X,
        TEXT_AREA_Y,
        TEXT_AREA_W,
        TEXT_AREA_H,
        KEY_RADIUS,
        COLOR_DARKGRAY
    );

    tft.setTextSize(2);
    tft.setTextColor(COLOR_WHITE);

    tft.setCursor(
        TEXT_CURSOR_X,
        TEXT_CURSOR_Y
    );

    tft.print(textBuffer);

    // Curseur
    int16_t cursorWidth = tft.width();

    int16_t approxCharWidth = 12;

    cursorWidth =
        TEXT_CURSOR_X +
        (textLength * approxCharWidth);

    if (cursorWidth > TEXT_AREA_X + TEXT_AREA_W - 5)
    {
        cursorWidth =
            TEXT_AREA_X +
            TEXT_AREA_W -
            5;
    }

    tft.drawFastVLine(
        cursorWidth,
        TEXT_AREA_Y + 8,
        TEXT_AREA_H - 16,
        COLOR_CYAN
    );
}

// ============================================================
// Dessin d'une touche
// ============================================================

static void drawKey(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const char* label,
    uint16_t color
)
{
    tft.fillRoundRect(
        x,
        y,
        w,
        h,
        KEY_RADIUS,
        color
    );

    tft.drawRoundRect(
        x,
        y,
        w,
        h,
        KEY_RADIUS,
        COLOR_WHITE
    );

    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(2);

    int16_t textWidth =
        strlen(label) * 12;

    int16_t textX =
        x + (w - textWidth) / 2;

    int16_t textY =
        y + (h - 16) / 2;

    if (textX < x + 2)
    {
        textX = x + 2;
    }

    tft.setCursor(
        textX,
        textY
    );

    tft.print(label);
}

// ============================================================
// Calcul ligne
// ============================================================

static void drawCharacterRow(
    const char* chars,
    int16_t y,
    int16_t h
)
{
    int count = strlen(chars);

    if (count <= 0)
    {
        return;
    }

    int16_t w =
        (
            SCREEN_WIDTH -
            (KEYBOARD_X * 2) -
            (KEY_GAP * (count - 1))
        ) / count;

    int16_t x =
        KEYBOARD_X;

    for (int i = 0; i < count; i++)
    {
        char label[2];

        label[0] = chars[i];
        label[1] = '\0';

        char displayLabel[2];

        if (shiftEnabled)
        {
            displayLabel[0] =
                toupper(chars[i]);
        }
        else
        {
            displayLabel[0] =
                tolower(chars[i]);
        }

        displayLabel[1] = '\0';

        drawKey(
            x,
            y,
            w,
            h,
            displayLabel,
            COLOR_DARKGRAY
        );

        x += w + KEY_GAP;
    }
}

// ============================================================
// Initialisation
// ============================================================

void keyboardBegin()
{
    Serial.println("[KEYBOARD] Initialisation");

    textBuffer[0] = '\0';
    textLength = 0;

    currentMode = KEYBOARD_ALPHA;

    shiftEnabled = false;

    validated = false;

    tft.begin();

    tft.setRotation(TFT_ROTATION);

    tft.fillScreen(COLOR_BLACK);

    Serial.println("[KEYBOARD] TFT OK");

    keyboardDraw();

    Serial.println("[KEYBOARD] Pret");
}

// ============================================================
// Affichage complet
// ============================================================

void keyboardDraw()
{
    tft.fillScreen(COLOR_BLACK);

    drawTextArea();

    // --------------------------------------------------------
    // ALPHA
    // --------------------------------------------------------

    if (currentMode == KEYBOARD_ALPHA)
    {
        const char* row1 = "qwertyuiop";
        const char* row2 = "asdfghjkl";
        const char* row3 = "zxcvbnm";

        int16_t rowH = 32;

        drawCharacterRow(
            row1,
            53,
            rowH
        );

        drawCharacterRow(
            row2,
            89,
            rowH
        );

        drawCharacterRow(
            row3,
            125,
            rowH
        );

        // SHIFT
        drawKey(
            5,
            161,
            52,
            32,
            shiftEnabled ? "UP" : "SHIFT",
            shiftEnabled
                ? COLOR_BLUE
                : COLOR_DARKGRAY
        );

        // Espace
        drawKey(
            61,
            161,
            140,
            32,
            "SPACE",
            COLOR_DARKGRAY
        );

        // Backspace
        drawKey(
            205,
            161,
            52,
            32,
            "<-",
            COLOR_DARKGRAY
        );

        // Enter
        drawKey(
            261,
            161,
            54,
            32,
            "OK",
            COLOR_GREEN
        );
    }

    // --------------------------------------------------------
    // NUMERIQUE
    // --------------------------------------------------------

    else if (currentMode == KEYBOARD_NUMERIC)
    {
        const char* row1 = "123";
        const char* row2 = "456";
        const char* row3 = "789";

        int16_t rowH = 32;

        drawCharacterRow(
            row1,
            53,
            rowH
        );

        drawCharacterRow(
            row2,
            89,
            rowH
        );

        drawCharacterRow(
            row3,
            125,
            rowH
        );

        drawKey(
            5,
            161,
            70,
            32,
            "ABC",
            COLOR_BLUE
        );

        drawKey(
            79,
            161,
            70,
            32,
            "0",
            COLOR_DARKGRAY
        );

        drawKey(
            153,
            161,
            70,
            32,
            "<-",
            COLOR_DARKGRAY
        );

        drawKey(
            227,
            161,
            88,
            32,
            "OK",
            COLOR_GREEN
        );
    }

    // --------------------------------------------------------
    // SYMBOLES
    // --------------------------------------------------------

    else
    {
        const char* row1 = "!@#$%^&*";
        const char* row2 = "()-_=+";
        const char* row3 = ".,:;?/";

        drawCharacterRow(
            row1,
            53,
            32
        );

        drawCharacterRow(
            row2,
            89,
            32
        );

        drawCharacterRow(
            row3,
            125,
            32
        );

        drawKey(
            5,
            161,
            70,
            32,
            "ABC",
            COLOR_BLUE
        );

        drawKey(
            79,
            161,
            70,
            32,
            "123",
            COLOR_BLUE
        );

        drawKey(
            153,
            161,
            70,
            32,
            "<-",
            COLOR_DARKGRAY
        );

        drawKey(
            227,
            161,
            88,
            32,
            "OK",
            COLOR_GREEN
        );
    }
}

// ============================================================
// Ajouter caractère
// ============================================================

static void addCharacter(char c)
{
    if (textLength >= MAX_TEXT_LENGTH)
    {
        return;
    }

    textBuffer[textLength] = c;

    textLength++;

    textBuffer[textLength] = '\0';

    Serial.print("[KEYBOARD] Ajout : ");
    Serial.println(c);

    keyboardDraw();
}

// ============================================================
// Retour arrière
// ============================================================

void keyboardBackspace()
{
    if (textLength == 0)
    {
        return;
    }

    textLength--;

    textBuffer[textLength] = '\0';

    Serial.println("[KEYBOARD] BACKSPACE");

    keyboardDraw();
}

// ============================================================
// Effacement
// ============================================================

void keyboardClearText()
{
    textLength = 0;

    textBuffer[0] = '\0';

    Serial.println("[KEYBOARD] CLEAR");

    keyboardDraw();
}

// ============================================================
// SHIFT
// ============================================================

void keyboardToggleShift()
{
    shiftEnabled = !shiftEnabled;

    Serial.print("[KEYBOARD] SHIFT = ");

    Serial.println(
        shiftEnabled ? "ON" : "OFF"
    );

    keyboardDraw();
}

// ============================================================
// Mode
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
)
{
    currentMode = mode;

    Serial.print("[KEYBOARD] MODE = ");

    if (mode == KEYBOARD_ALPHA)
    {
        Serial.println("ALPHA");
    }
    else if (mode == KEYBOARD_NUMERIC)
    {
        Serial.println("NUMERIC");
    }
    else
    {
        Serial.println("SYMBOLS");
    }

    keyboardDraw();
}

KeyboardMode keyboardGetMode()
{
    return currentMode;
}

// ============================================================
// Etat SHIFT
// ============================================================

bool keyboardIsShift()
{
    return shiftEnabled;
}

// ============================================================
// Etat validation
// ============================================================

bool keyboardWasValidated()
{
    return validated;
}

void keyboardClearValidated()
{
    validated = false;
}

// ============================================================
// Texte
// ============================================================

const char* keyboardGetText()
{
    return textBuffer;
}

// ============================================================
// Gestion tactile
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print("[KEYBOARD] TOUCH X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);

    // --------------------------------------------------------
    // Vérification zone clavier
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

    // --------------------------------------------------------
    // ALPHA
    // --------------------------------------------------------

    if (currentMode == KEYBOARD_ALPHA)
    {
        // Ligne 1
        if (y >= 53 && y < 85)
        {
            const char* row = "qwertyuiop";

            int count = 10;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    10 -
                    10 -
                    KEY_GAP * 9
                ) / 10;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    char c = row[index];

                    if (shiftEnabled)
                    {
                        c = toupper(c);
                    }

                    addCharacter(c);

                    if (shiftEnabled)
                    {
                        shiftEnabled = false;
                    }
                }
            }

            return;
        }

        // Ligne 2
        if (y >= 89 && y < 121)
        {
            const char* row = "asdfghjkl";

            int count = 9;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    10 -
                    10 -
                    KEY_GAP * 8
                ) / 9;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    char c = row[index];

                    if (shiftEnabled)
                    {
                        c = toupper(c);
                    }

                    addCharacter(c);

                    if (shiftEnabled)
                    {
                        shiftEnabled = false;
                    }
                }
            }

            return;
        }

        // Ligne 3
        if (y >= 125 && y < 157)
        {
            const char* row = "zxcvbnm";

            int count = 7;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    10 -
                    10 -
                    KEY_GAP * 6
                ) / 7;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    char c = row[index];

                    if (shiftEnabled)
                    {
                        c = toupper(c);
                    }

                    addCharacter(c);

                    if (shiftEnabled)
                    {
                        shiftEnabled = false;
                    }
                }
            }

            return;
        }

        // Ligne fonctionnelle
        if (y >= 161 && y < 193)
        {
            // SHIFT
            if (x >= 5 && x < 57)
            {
                keyboardToggleShift();
                return;
            }

            // SPACE
            if (x >= 61 && x < 201)
            {
                addCharacter(' ');
                return;
            }

            // BACKSPACE
            if (x >= 205 && x < 257)
            {
                keyboardBackspace();
                return;
            }

            // OK
            if (x >= 261 && x < 315)
            {
                validated = true;

                Serial.print("[KEYBOARD] VALIDE : ");
                Serial.println(textBuffer);

                keyboardDraw();

                return;
            }
        }
    }

    // --------------------------------------------------------
    // NUMERIC
    // --------------------------------------------------------

    else if (currentMode == KEYBOARD_NUMERIC)
    {
        // Ligne 1
        if (y >= 53 && y < 85)
        {
            const char* row = "123";

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 2
                ) / 3;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < 3)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        // Ligne 2
        if (y >= 89 && y < 121)
        {
            const char* row = "456";

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 2
                ) / 3;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < 3)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        // Ligne 3
        if (y >= 125 && y < 157)
        {
            const char* row = "789";

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 2
                ) / 3;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < 3)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        if (y >= 161 && y < 193)
        {
            if (x >= 5 && x < 75)
            {
                keyboardSetMode(
                    KEYBOARD_ALPHA
                );

                return;
            }

            if (x >= 79 && x < 149)
            {
                addCharacter('0');
                return;
            }

            if (x >= 153 && x < 223)
            {
                keyboardBackspace();
                return;
            }

            if (x >= 227 && x < 315)
            {
                validated = true;

                Serial.print("[KEYBOARD] VALIDE : ");
                Serial.println(textBuffer);

                return;
            }
        }
    }

    // --------------------------------------------------------
    // SYMBOLS
    // --------------------------------------------------------

    else
    {
        if (y >= 53 && y < 85)
        {
            const char* row = "!@#$%^&*";

            int count = 8;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 7
                ) / 8;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        if (y >= 89 && y < 121)
        {
            const char* row = "()-_=+";

            int count = 6;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 5
                ) / 6;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        if (y >= 125 && y < 157)
        {
            const char* row = ".,:;?/";

            int count = 6;

            int16_t w =
                (
                    SCREEN_WIDTH -
                    20 -
                    KEY_GAP * 5
                ) / 6;

            int index =
                (x - 5) /
                (w + KEY_GAP);

            if (index >= 0 && index < count)
            {
                int16_t keyX =
                    5 +
                    index *
                    (w + KEY_GAP);

                if (x < keyX + w)
                {
                    addCharacter(row[index]);
                }
            }

            return;
        }

        if (y >= 161 && y < 193)
        {
            if (x >= 5 && x < 75)
            {
                keyboardSetMode(
                    KEYBOARD_ALPHA
                );

                return;
            }

            if (x >= 79 && x < 149)
            {
                keyboardSetMode(
                    KEYBOARD_NUMERIC
                );

                return;
            }

            if (x >= 153 && x < 223)
            {
                keyboardBackspace();
                return;
            }

            if (x >= 227 && x < 315)
            {
                validated = true;

                Serial.print("[KEYBOARD] VALIDE : ");
                Serial.println(textBuffer);

                return;
            }
        }
    }
}