#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration générale
// ============================================================


// ============================================================
// ÉCRAN
// ============================================================

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  320
#define SCREEN_ROTATION 2


// ============================================================
// CALIBRATION TACTILE
// ============================================================

// 1 = afficher le choix de calibration au démarrage
// 0 = utiliser directement les anciennes valeurs
#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Anciennes valeurs de calibration
//
// Valeurs relevées précédemment :
//
// CAL0 = 294
// CAL1 = 3385
// CAL2 = 442
// CAL3 = 289
// CAL4 = 3387
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0  294
#define TOUCH_PREVIOUS_CAL_1  3385
#define TOUCH_PREVIOUS_CAL_2  442
#define TOUCH_PREVIOUS_CAL_3  289
#define TOUCH_PREVIOUS_CAL_4  3387


// ============================================================
// ÉCRAN DE CHOIX DE CALIBRATION
// ============================================================

// Bouton : NOUVELLE CALIBRATION

#define CALIBRATION_NEW_X     10
#define CALIBRATION_NEW_Y     105
#define CALIBRATION_NEW_W     220
#define CALIBRATION_NEW_H     70


// Bouton : CONFIGURATION PRÉCÉDENTE

#define CALIBRATION_REUSE_X   10
#define CALIBRATION_REUSE_Y   190
#define CALIBRATION_REUSE_W   220
#define CALIBRATION_REUSE_H   70


// ============================================================
// ZONE CLAVIER
// ============================================================

#define KEYBOARD_X     0
#define KEYBOARD_Y     138
#define KEYBOARD_W     240
#define KEYBOARD_HEIGHT 182


// Nombre de lignes et colonnes

#define KEYBOARD_ROWS  4
#define KEYBOARD_COLS  10


// ============================================================
// CHAMP DE SAISIE
// ============================================================

#define INPUT_X        10
#define INPUT_Y        10
#define INPUT_W        150
#define INPUT_H        50


// ============================================================
// BOUTON DEL
// ============================================================

#define BTN_DEL_X      165
#define BTN_DEL_Y      10
#define BTN_DEL_W      30
#define BTN_DEL_H      50


// ============================================================
// BOUTON OK
// ============================================================

#define BTN_OK_X       200
#define BTN_OK_Y       10
#define BTN_OK_W       30
#define BTN_OK_H       50


// ============================================================
// TEXTE
// ============================================================

#define KEYBOARD_MAX_TEXT 64


// ============================================================
// COULEURS
// ============================================================

#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_GREY        0x7BEF
#define COLOR_DARK_GREY   0x39E7
#define COLOR_BLUE        0x001F
#define COLOR_CYAN        0x07FF
#define COLOR_GREEN       0x07E0
#define COLOR_RED         0xF800
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20


#endif