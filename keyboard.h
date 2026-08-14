#ifndef BO4RD_KEYBOARD_H
#define BO4RD_KEYBOARD_H

#include <Arduino.h>

// ============================================================
// MODES CLAVIER
// ============================================================

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_SYMBOLS = 1
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
// TRAITEMENT TACTILE
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
);

// ============================================================
// TEXTE
// ============================================================

const String& keyboardGetText();

void keyboardClear();

// ============================================================
// COMMANDES
// ============================================================

void keyboardDelete();

void keyboardCursorLeft();

void keyboardCursorRight();

void keyboardValidate();

// ============================================================
// MODE
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
);

KeyboardMode keyboardGetMode();

#endif