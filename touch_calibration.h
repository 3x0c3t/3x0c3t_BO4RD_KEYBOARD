#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// Initialisation du tactile
void touchCalibrationBegin();

// Lecture tactile et conversion en coordonnées écran
bool touchReadScreen(int16_t &screenX, int16_t &screenY);

// Lecture brute pour diagnostic
bool touchReadRaw(
    int16_t &rawX,
    int16_t &rawY,
    int16_t &rawZ
);

#endif