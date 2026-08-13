#ifndef KEYBOARD_MODULE_H
#define KEYBOARD_MODULE_H

#include <Arduino.h>

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Interface clavier
// ============================================================

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC
};

// ------------------------------------------------------------
// Initialisation
// ------------------------------------------------------------

void keyboardInit();
void keyboardBegin();

// ------------------------------------------------------------
// Affichage
// ------------------------------------------------------------

void keyboardDraw();

// ------------------------------------------------------------
// Gestion tactile
// ------------------------------------------------------------

void keyboardUpdate(int16_t x, int16_t y);

// ------------------------------------------------------------
// Gestion du texte
// ------------------------------------------------------------

void keyboardDelete();
void keyboardValidate();
void keyboardClear();

const char* keyboardGetText();
void keyboardSetText(const char* text);

bool keyboardHasText();

// ------------------------------------------------------------
// Mode clavier
// ------------------------------------------------------------

void keyboardSetMode(KeyboardMode mode);
KeyboardMode keyboardGetMode();

#endif