#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>
#include <TFT_eSPI.h>


// ============================================================
// DONNÉES CALIBRATION
// ============================================================

extern uint16_t calData[5];


// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();


// ============================================================
// CHOIX CALIBRATION
// ============================================================

bool touchCalibrationChoice(
    uint16_t x,
    uint16_t y,
    uint8_t &choice
);

bool touchCalibrationChoiceActive();


// ============================================================
// CALIBRATION
// ============================================================

void startTouchCalibration();


// ============================================================
// CONFIGURATION PRÉCÉDENTE
// ============================================================

void loadPreviousCalibration();


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