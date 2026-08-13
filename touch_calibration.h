#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

void touchCalibrationInit();

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

bool touchCalibrationIsFinished();

uint16_t *touchCalibrationData();

void loadPreviousCalibration();

#endif