#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();

// ============================================================
// ECRAN DE CHOIX
// ============================================================

void touchCalibrationChoice();

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive();

bool touchCalibrationFinished();

// ============================================================
// TRAITEMENT TOUCH
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration();

bool loadPreviousCalibration();

// ============================================================
// ZONES
// ============================================================

bool isNewCalibrationZone(
    uint16_t x,
    uint16_t y
);

bool isPreviousCalibrationZone(
    uint16_t x,
    uint16_t y
);

#endif