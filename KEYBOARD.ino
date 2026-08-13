#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

#ifndef TFT_CS
#define TFT_CS 15
#endif

#ifndef TFT_DC
#define TFT_DC 2
#endif

#ifndef TFT_RST
#define TFT_RST 16
#endif

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("==============================");
    Serial.println("  3x0c3t KEYBOARD v1.0");
    Serial.println("==============================");

    SPI.begin();

    Serial.println("[KEYBOARD] TFT Init");

    tft.begin();
    tft.setRotation(2);

    tft.fillScreen(ILI9341_BLACK);

    delay(300);

    tft.fillScreen(ILI9341_BLUE);

    delay(300);

    tft.fillScreen(ILI9341_BLACK);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 20);
    tft.print("3x0c3t KEYBOARD");

    tft.setTextSize(1);
    tft.setCursor(20, 50);
    tft.print("TFT OK");

    Serial.println("[KEYBOARD] TFT OK");

    touchCalibrationBegin();

    keyboardBegin();

    Serial.println("[KEYBOARD] Pret");
    Serial.println("[KEYBOARD] OK");
    Serial.println("==============================");
    Serial.println("  KEYBOARD READY");
    Serial.println("==============================");
}

void loop()
{
    int16_t x = -1;
    int16_t y = -1;

    if (touchReadScreen(&x, &y))
    {
        Serial.print("[TOUCH] X=");
        Serial.print(x);
        Serial.print(" Y=");
        Serial.println(y);

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