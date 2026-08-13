#ifndef TOUCH_CALIBRATION_H
#define TOUCH_CALIBRATION_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ETATS
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

// Retourne true si une calibration doit être lancée.
// Retourne false si la configuration précédente est utilisée.
bool touchCalibrationChoice();

// ============================================================
// TRAITEMENT DU TOUCH
// ============================================================

bool touchCalibrationUpdate(
    uint16_t x,
    uint16_t y
);

// ============================================================
// CONFIGURATION PRECEDENTE
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