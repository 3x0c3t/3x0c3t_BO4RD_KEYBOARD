#ifndef BO4RD_TOUCH_CALIBRATION_H
#define BO4RD_TOUCH_CALIBRATION_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ETAT CALIBRATION
// ============================================================

enum TouchCalibrationState
{
    TOUCH_CALIBRATION_CHOICE = 0,
    TOUCH_CALIBRATION_RUNNING,
    TOUCH_CALIBRATION_DONE,
    TOUCH_CALIBRATION_REUSE,
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
// TRAITEMENT TOUCH
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

// ============================================================
// ANCIENNE CALIBRATION
// ============================================================

bool loadPreviousCalibration();

// ============================================================
// NOUVELLE CALIBRATION
// ============================================================

void startTouchCalibration();

// ============================================================
// ETAT
// ============================================================

bool touchCalibrationActive();

bool touchCalibrationFinished();

#endif