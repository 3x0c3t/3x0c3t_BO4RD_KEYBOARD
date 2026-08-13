#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>

// ------------------------------------------------------------
// Initialisation
// ------------------------------------------------------------

void touchCalibrationInit();


// ------------------------------------------------------------
// Choix utilisateur
//
// Retourne true lorsque le choix est termine.
// ------------------------------------------------------------

bool touchCalibrationChoice(
    uint16_t x,
    uint16_t y
);


// ------------------------------------------------------------
// Mise a jour pendant la calibration
//
// Retourne true lorsque la calibration est terminee.
// ------------------------------------------------------------

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);


// ------------------------------------------------------------
// Lancement d'une nouvelle calibration
// ------------------------------------------------------------

void startTouchCalibration();


// ------------------------------------------------------------
// Charger la calibration precedente
// ------------------------------------------------------------

void loadPreviousCalibration();


// ------------------------------------------------------------
// Appliquer la calibration precedente
// ------------------------------------------------------------

void applyPreviousCalibration();


// ------------------------------------------------------------
// Etat
// ------------------------------------------------------------

bool touchCalibrationIsWaiting();

bool touchCalibrationIsRunning();

bool touchCalibrationIsFinished();

bool touchCalibrationUsePrevious();


#endif