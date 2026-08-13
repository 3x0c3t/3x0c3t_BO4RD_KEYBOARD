#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

void touchCalibration();

void touchCalibrationScreen();

bool touchReusePreviousCalibration();

void loadPreviousCalibration();

#endif