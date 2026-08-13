#include "keyboard.h"
#include "config.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;


// ============================================================
// Mode clavier
// ============================================================

static KeyboardMode currentMode = KEYBOARD_ALPHA;


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