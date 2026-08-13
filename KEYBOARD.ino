#include <Arduino.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

TFT_eSPI tft = TFT_eSPI();

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("=== 3x0c3t KEYBOARD v1.0 ===");

    Serial.println("[KEYBOARD] TFT Init");

    tft.init();
    tft.setRotation(SCREEN_ROTATION);
    tft.fillScreen(COLOR_BLACK);

    Serial.println("[KEYBOARD] TFT OK");

    touchCalibrationInit();

    if (touchCalibrationChoice())
    {
        keyboardInit();
        keyboardDraw();
    }
}

void loop()
{
    uint16_t x;
    uint16_t y;

    if (tft.getTouch(&x, &y))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);
        Serial.print(" Y=");
        Serial.println(y);

        touchCalibrationUpdate(x, y);

        delay(120);
    }
}