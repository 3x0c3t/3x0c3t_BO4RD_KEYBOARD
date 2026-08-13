#include <Arduino.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("==============================");
    Serial.println("  3x0c3t KEYBOARD v1.0");
    Serial.println("==============================");

    touchCalibrationBegin();
    keyboardBegin();

    Serial.println("[KEYBOARD] Ready");
}

void loop()
{
    int16_t x = -1;
    int16_t y = -1;

    if (touchReadScreen(x, y))
    {
        keyboardUpdate(x, y);
    }

    if (keyboardWasValidated())
    {
        Serial.print("[KEYBOARD] Texte valide : ");
        Serial.println(keyboardGetText());

        keyboardClearValidated();
    }

    delay(10);
}