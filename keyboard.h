#ifndef BO4RD_KEYBOARD_MODULE_H
#define BO4RD_KEYBOARD_MODULE_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// MODES
// ============================================================

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC,
    KEYBOARD_SYMBOLS
};

// ============================================================
// INITIALISATION
// ============================================================

void keyboardInit();

void keyboardBegin();

// ============================================================
// AFFICHAGE
// ============================================================

void keyboardDraw();

// ============================================================
// TRAITEMENT TOUCH
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
);

// ============================================================
// MODE
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
);

KeyboardMode keyboardGetMode();

// ============================================================
// ACTIONS
// ============================================================

void keyboardDelete();

void keyboardValidate();

void keyboardClear();

// ============================================================
// TEXTE
// ============================================================

const String& keyboardGetText();

#endif