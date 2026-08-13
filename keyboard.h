#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <Arduino.h>

enum KeyboardMode
{
    KEYBOARD_ALPHA = 0,
    KEYBOARD_NUMERIC = 1
};

void keyboardBegin();
void keyboardDraw();
void keyboardUpdate(int16_t x, int16_t y);

void keyboardSetMode(KeyboardMode mode);
KeyboardMode keyboardGetMode();

const char* keyboardGetText();

bool keyboardWasValidated();
void keyboardClearValidated();

void keyboardClearText();

#endif