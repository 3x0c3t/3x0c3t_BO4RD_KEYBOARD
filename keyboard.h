#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// MODES DU CLAVIER
// ============================================================

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC,
    KEYBOARD_SYMBOLS
};

// ============================================================
// API CLAVIER
// ============================================================

void keyboardInit();

void keyboardBegin();

void keyboardDraw();

void keyboardUpdate(
    int16_t x,
    int16_t y
);

void keyboardSetMode(
    KeyboardMode mode
);

KeyboardMode keyboardGetMode();

void keyboardDelete();

void keyboardValidate();

const String& keyboardGetText();

void keyboardClear();

#endif