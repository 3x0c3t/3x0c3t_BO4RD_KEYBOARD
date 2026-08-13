#ifndef KEYBOARD_MODULE_H
#define KEYBOARD_MODULE_H

#include <Arduino.h>

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC = 1
};

void keyboardBegin();

void keyboardDraw();

void keyboardUpdate(int16_t x, int16_t y);

const char* keyboardGetText();

void keyboardClearText();

bool keyboardWasValidated();

void keyboardClearValidated();

void keyboardSetMode(KeyboardMode mode);

KeyboardMode keyboardGetMode();

#endif