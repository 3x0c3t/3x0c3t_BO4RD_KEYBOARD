#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();

// ============================================================
// DEMARRAGE CALIBRATION
// ============================================================

void startTouchCalibration();

// ============================================================
// UPDATE CALIBRATION
// ============================================================
//
// Lecture du tactile et traitement automatique.
//

void updateTouchCalibration();

// ============================================================
// TRAITEMENT AVEC COORDONNEES
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
// ORIENTATION
// ============================================================

uint8_t selectedTouchRotation();

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