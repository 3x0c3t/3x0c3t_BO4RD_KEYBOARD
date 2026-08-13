#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// Initialise le contrôleur tactile
void touchCalibrationBegin();

// Lit le tactile et retourne les coordonnées écran
// Retourne true si un appui valide est détecté
bool touchReadScreen(int16_t* x, int16_t* y);

#endif