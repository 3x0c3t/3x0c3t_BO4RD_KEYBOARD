#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ============================================================
// Initialisation du tactile
// ============================================================

void touchCalibrationBegin();

// ============================================================
// Lecture tactile
// ============================================================
//
// Retourne true lorsqu'un contact est détecté.
//
// x et y sont les coordonnées écran :
// X = 0..239
// Y = 0..319
//
// ============================================================

bool touchReadScreen(
    int16_t* x,
    int16_t* y
);

#endif