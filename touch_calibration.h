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

// ============================================================
// EEPROM
// ============================================================

bool loadTouchCalibration();

bool saveTouchCalibration();

bool clearTouchCalibration();

// ============================================================
// COMPATIBILITE
// ============================================================

bool touchCalibrationChoice();

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

bool loadPreviousCalibration();

bool touchCalibrationActive();

bool touchCalibrationFinished();

#endif