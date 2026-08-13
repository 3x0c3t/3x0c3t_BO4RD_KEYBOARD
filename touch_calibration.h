#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// ETAT
// ============================================================

enum TouchCalibrationState
{
    TOUCH_CALIBRATION_CHOICE = 0,
    TOUCH_CALIBRATION_RUNNING,
    TOUCH_CALIBRATION_DONE,
    TOUCH_CALIBRATION_KEYBOARD
};

// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();

// ============================================================
// ECRAN DE CHOIX
// ============================================================

bool touchCalibrationChoice();

// ============================================================
// MISE A JOUR
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration();

// ============================================================
// CONFIGURATION PRECEDENTE
// ============================================================

bool touchReusePreviousCalibration();

void loadPreviousCalibration();

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationIsRunning();

bool touchCalibrationIsComplete();

bool touchCalibrationUseKeyboard();

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