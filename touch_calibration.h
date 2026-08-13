#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Initialise la calibration tactile
void touchCalibration();

// Effectue une calibration complète
void touchCalibrate();

// Affiche l'écran de calibration
void drawCalibrationScreen();

// Affiche une cible cercle + croix
void drawCalibrationTarget(
    int16_t x,
    int16_t y
);

// Teste si l'utilisateur a touché
// le bouton de reprise de l'ancienne calibration
bool touchReusePreviousCalibration();

// Charge les anciennes valeurs de calibration
void loadPreviousCalibration();

// Affiche les valeurs de calibration
void printCalibrationData();

#endif