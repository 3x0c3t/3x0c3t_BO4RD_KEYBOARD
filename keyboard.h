#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>

// ============================================================
// Modes clavier
// ============================================================

enum KeyboardMode
{
    KEYBOARD_ALPHA,
    KEYBOARD_NUMERIC,
    KEYBOARD_SYMBOLS
};

// ============================================================
// Initialisation
// ============================================================

void keyboardBegin();

// ============================================================
// Affichage
// ============================================================

void keyboardDraw();

// ============================================================
// Gestion tactile
// ============================================================

void keyboardUpdate(
    int16_t x,
    int16_t y
);

// ============================================================
// Texte
// ============================================================

const char* keyboardGetText();

void keyboardClearText();

void keyboardBackspace();

// ============================================================
// Mode clavier
// ============================================================

void keyboardSetMode(
    KeyboardMode mode
);

KeyboardMode keyboardGetMode();

// ============================================================
// SHIFT
// ============================================================

void keyboardToggleShift();

bool keyboardIsShift();

// ============================================================
// Validation
// ============================================================

bool keyboardWasValidated();

void keyboardClearValidated();

#endif