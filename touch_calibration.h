#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

void touchCalibrationBegin();

bool touchReadScreen(
    int16_t* x,
    int16_t* y
);

#endif