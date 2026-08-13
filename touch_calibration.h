#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>


// ============================================================
// DONNEES DE CALIBRATION
// ============================================================

extern uint16_t calData[5];


// ============================================================
// INITIALISATION
// ============================================================

void touchCalibrationInit();


// ============================================================
// CALIBRATION
// ============================================================

void touchCalibrate();


// ============================================================
// AFFICHAGE
// ============================================================

void drawCalibrationScreen();


// ============================================================
// ANCIENNE CALIBRATION
// ============================================================

bool touchReusePreviousCalibration(
    int16_t x,
    int16_t y
);

void loadPreviousCalibration();

#endif