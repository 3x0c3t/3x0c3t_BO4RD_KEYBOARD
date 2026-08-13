#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include "config.h"
#include "keyboard.h"
#include "touch_calibration.h"

TFT_eSPI tft = TFT_eSPI();

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("  3x0c3t KEYBOARD v1.0");
    Serial.println("==============================");

    Serial.println("[KEYBOARD] TFT Init");

    tft.init();

    Serial.println("[KEYBOARD] TFT OK");

    tft.setRotation(2);

    Serial.println("[KEYBOARD] Rotation OK");

    tft.fillScreen(TFT_BLACK);

    Serial.println("[KEYBOARD] Ecran noir OK");

    delay(500);

    Serial.println("[KEYBOARD] Test ROUGE");
    tft.fillScreen(TFT_RED);
    delay(500);

    Serial.println("[KEYBOARD] Test VERT");
    tft.fillScreen(TFT_GREEN);
    delay(500);

    Serial.println("[KEYBOARD] Test BLEU");
    tft.fillScreen(TFT_BLUE);
    delay(500);

    Serial.println("[KEYBOARD] Test NOIR");
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(20, 20);
    tft.println("3x0c3t");
    tft.println();
    tft.println("KEYBOARD");
    tft.println();
    tft.println("TFT OK");

    Serial.println("[TOUCH] Init");

    touchCalibrationBegin();

    Serial.println("[TOUCH] OK");

    Serial.println("[KEYBOARD] Initialisation");

    keyboardBegin();

    Serial.println("[KEYBOARD] OK");

    Serial.println("==============================");
    Serial.println("  KEYBOARD READY");
    Serial.println("==============================");
}

void loop()
{
    int16_t x;
    int16_t y;

    if (touchReadScreen(&x, &y))
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