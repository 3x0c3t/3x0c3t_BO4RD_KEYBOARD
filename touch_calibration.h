#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();

// ============================================================
// DEMARRAGE
// ============================================================

void startTouchCalibration();

// ============================================================
// TRAITEMENT TOUCH
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive();

bool touchCalibrationFinished();

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

bool loadPreviousCalibration();

#endif