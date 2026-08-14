#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

void touchCalibrationInit();

void startTouchCalibration();

void updateTouchCalibration();

bool touchCalibrationActive();

bool touchCalibrationFinished();

bool loadTouchCalibration();

bool saveTouchCalibration();

bool clearTouchCalibration();

bool touchCalibrationChoice();

bool loadPreviousCalibration();

uint8_t selectedTouchRotation();

#endif