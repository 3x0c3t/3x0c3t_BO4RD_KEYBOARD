#ifndef KEYBOARD_MODULE_H
#define KEYBOARD_MODULE_H

#include <Arduino.h>

enum KeyboardMode
{
    KEYBOARD_ALPHA,
    KEYBOARD_NUMERIC
};

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

const char* keyboardGetText();

void keyboardClearText();

#endif