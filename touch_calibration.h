#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();

// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration();

void updateTouchCalibration();

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive();

bool touchCalibrationFinished();

bool touchCalibrationChoice();

// ============================================================
// ROTATION
// ============================================================

uint8_t selectedTouchRotation();

// ============================================================
// EEPROM
// ============================================================

bool loadTouchCalibration();

bool saveTouchCalibration();

bool clearTouchCalibration();

bool loadPreviousCalibration();

#endif