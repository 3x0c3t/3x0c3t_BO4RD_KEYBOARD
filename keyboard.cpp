#include "keyboard.h"
#include "config.h"

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static KeyboardMode currentMode = KEYBOARD_ALPHA;

static String keyboardText = "";

static const char alphaRows[4][11] =
{
    "ABCDEFGHIJ",
    "KLMNOPQRST",
    "UVWXYZ0123",
    "456789-_ ."
};

static const char numericRows[4][11] =
{
    "1234567890",
    "-_.:/+*#@ ",
    "ABCDEFGHIJ",
    "KLMNOPQRST"
};

static int16_t keyWidth()
{
    return KEYBOARD_W / KEYBOARD_COLS;
}

static int16_t keyHeight()
{
    return KEYBOARD_H / KEYBOARD_ROWS;
}

static bool inside(
    int16_t x,
    int16_t y,
    int16_t rx,
    int16_t ry,
    int16_t rw,
    int16_t rh
)
{
    return (
        x >= rx &&
        x < rx + rw &&
        y >= ry &&
        y < ry + rh
    );
}

static void drawInputArea()
{
    tft.fillRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        TFT_BLACK
    );

    tft.drawRect(
        INPUT_X,
        INPUT_Y,
        INPUT_W,
        INPUT_H,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.drawString(
        keyboardText,
        INPUT_X + INPUT_W / 2,
        INPUT_Y + INPUT_H / 2,
        2
    );
}

static void drawDeleteButton()
{
    tft.fillRect(
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H,
        TFT_RED
    );

    tft.drawRect(
        BTN_DEL_X,
        BTN_DEL_Y,
        BTN_DEL_W,
        BTN_DEL_H,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_RED
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.drawString(
        "DEL",
        BTN_DEL_X + BTN_DEL_W / 2,
        BTN_DEL_Y + BTN_DEL_H / 2,
        2
    );
}

static void drawOKButton()
{
    tft.fillRect(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        TFT_GREEN
    );

    tft.drawRect(
        BTN_OK_X,
        BTN_OK_Y,
        BTN_OK_W,
        BTN_OK_H,
        TFT_WHITE
    );

    tft.setTextColor(
        TFT_BLACK,
        TFT_GREEN
    );

    tft.setTextDatum(
        MC_DATUM
    );

    tft.drawString(
        "OK",
        BTN_OK_X + BTN_OK_W / 2,
        BTN_OK_Y + BTN_OK_H / 2,
        2
    );
}

void keyboardDraw()
{
    tft.fillScreen(TFT_BLACK);

    drawInputArea();

    const char (*rows)[KEYBOARD_COLS + 1];

    if (currentMode == KEYBOARD_ALPHA)
    {
        rows = alphaRows;
    }
    else
    {
        rows = numericRows;
    }

    const int16_t kw = keyWidth();
    const int16_t kh = keyHeight();

    for (int row = 0; row < KEYBOARD_ROWS; row++)
    {
        for (int col = 0; col < KEYBOARD_COLS; col++)
        {
            int16_t x = KEYBOARD_X + col * kw;
            int16_t y = KEYBOARD_Y + row * kh;

            tft.fillRect(
                x,
                y,
                kw - 2,
                kh - 2,
                TFT_DARKGREY
            );

            tft.drawRect(
                x,
                y,
                kw - 2,
                kh - 2,
                TFT_WHITE
            );

            char key = rows[row][col];

            if (key == '\0')
            {
                continue;
            }

            char text[2];

            text[0] = key;
            text[1] = '\0';

            tft.setTextDatum(
                MC_DATUM
            );

            tft.setTextColor(
                TFT_WHITE,
                TFT_DARKGREY
            );

            tft.drawString(
                text,
                x + kw / 2,
                y + kh / 2,
                2
            );
        }
    }

    drawDeleteButton();
    drawOKButton();

    tft.setTextDatum(
        TL_DATUM
    );
}

void keyboardInit()
{
    Serial.println("[KEYBOARD] Init");

    currentMode = KEYBOARD_ALPHA;
    keyboardText = "";

    keyboardDraw();

    Serial.println("[KEYBOARD] Init OK");
}

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

const char* keyboardGetText()
{
    return keyboardText.c_str();
}

void keyboardClear()
{
    keyboardText = "";

    drawInputArea();
}

static void keyboardDelete()
{
    if (keyboardText.length() == 0)
    {
        return;
    }

    keyboardText.remove(
        keyboardText.length() - 1
    );

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

static void keyboardValidate()
{
    Serial.print(
        "[KEYBOARD] Validation : "
    );

    Serial.println(
        keyboardText
    );

    if (keyboardText.length() == 0)
    {
        Serial.println(
            "[KEYBOARD] Texte vide"
        );

        return;
    }

    Serial.print(
        "[KEYBOARD] Texte valide : "
    );

    Serial.println(
        keyboardText
    );
}

void keyboardUpdate(
    int16_t x,
    int16_t y
)
{
    Serial.print(
        "[KEYBOARD] Analyse X="
    );

    Serial.print(x);

    Serial.print(
        " Y="
    );

    Serial.println(y);

    if (
        inside(
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
            "[KEYBOARD] ZONE = DEL"
        );

        keyboardDelete();

        return;
    }

    if (
        inside(
            x,
            y,
            BTN_OK_X,
            BTN_OK_Y,
            BTN_OK_W,
            BTN_OK_H
        )
    )
    {
        Serial.println(
            "[KEYBOARD] ZONE = OK"
        );

        keyboardValidate();

        return;
    }

    if (
        x < KEYBOARD_X ||
        x >= KEYBOARD_X + KEYBOARD_W ||
        y < KEYBOARD_Y ||
        y >= KEYBOARD_Y + KEYBOARD_H
    )
    {
        Serial.println(
            "[KEYBOARD] ZONE = HORS CLAVIER"
        );

        return;
    }

    const int16_t kw = keyWidth();
    const int16_t kh = keyHeight();

    int col =
        (x - KEYBOARD_X) / kw;

    int row =
        (y - KEYBOARD_Y) / kh;

    if (col < 0)
    {
        col = 0;
    }

    if (col >= KEYBOARD_COLS)
    {
        col = KEYBOARD_COLS - 1;
    }

    if (row < 0)
    {
        row = 0;
    }

    if (row >= KEYBOARD_ROWS)
    {
        row = KEYBOARD_ROWS - 1;
    }

    Serial.print(
        "[KEYBOARD] ROW="
    );

    Serial.print(row);

    Serial.print(
        " COL="
    );

    Serial.println(col);

    const char (*rows)[KEYBOARD_COLS + 1];

    if (currentMode == KEYBOARD_ALPHA)
    {
        rows = alphaRows;
    }
    else
    {
        rows = numericRows;
    }

    char key =
        rows[row][col];

    if (key == '\0')
    {
        return;
    }

    Serial.print(
        "[KEYBOARD] TOUCHE = "
    );

    Serial.println(key);

    if (key == ' ')
    {
        if (keyboardText.length() < KEYBOARD_MAX_LENGTH)
        {
            keyboardText += ' ';
        }
    }
    else
    {
        if (keyboardText.length() < KEYBOARD_MAX_LENGTH)
        {
            keyboardText += key;
        }
    }

    Serial.print(
        "[KEYBOARD] TEXTE = \""
    );

    Serial.print(
        keyboardText
    );

    Serial.println(
        "\""
    );

    drawInputArea();
}