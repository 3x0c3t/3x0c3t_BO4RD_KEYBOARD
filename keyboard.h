#ifndef BO4RD_KEYBOARD_CONFIG_H
#define BO4RD_KEYBOARD_CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ECRAN TFT
// ============================================================

#define SCREEN_WIDTH       240
#define SCREEN_HEIGHT      320
#define SCREEN_ROTATION    2

// ============================================================
// TOUCH
// ============================================================

#define TOUCH_THRESHOLD    600

// ============================================================
// EEPROM
// ============================================================

#define TOUCH_EEPROM_SIZE       64
#define TOUCH_EEPROM_MAGIC      0x3C7A
#define TOUCH_EEPROM_VERSION    1
#define TOUCH_EEPROM_ADDRESS    0

// ============================================================
// CALIBRATION
// ============================================================

#define FORCE_TOUCH_CALIBRATION    1

// ============================================================
// COULEURS
// ============================================================

#define COLOR_BLACK        TFT_BLACK
#define COLOR_WHITE        TFT_WHITE
#define COLOR_RED          TFT_RED
#define COLOR_GREEN        TFT_GREEN
#define COLOR_BLUE         TFT_BLUE
#define COLOR_YELLOW       TFT_YELLOW
#define COLOR_CYAN         TFT_CYAN
#define COLOR_MAGENTA      TFT_MAGENTA
#define COLOR_GREY         TFT_DARKGREY

// ============================================================
// CLAVIER
// ============================================================
//
// Ecran : 240 x 320
//
// Zone saisie :
//     0 .. 49
//
// Clavier :
//     60 .. 315
//
// 4 lignes :
//     A-I
//     J-R
//     S-Z
//     0-9 + SYM
//
// ============================================================

#define KEYBOARD_X             0
#define KEYBOARD_Y             60

#define KEYBOARD_W             SCREEN_WIDTH
#define KEYBOARD_HEIGHT        255

#define KEYBOARD_ROWS          4
#define KEYBOARD_COLS          10

#define KEYBOARD_MAX_LENGTH    64

// ============================================================
// ZONE DE SAISIE
// ============================================================

#define INPUT_X                5
#define INPUT_Y                5
#define INPUT_W                230
#define INPUT_H                48

// ============================================================
// CONTROLES DANS LA ZONE DE SAISIE
// ============================================================
//
// Ligne supérieure :
//     texte
//
// Ligne inférieure :
//     SUPPRIMER | GAUCHE | DROITE | OK
//
// ============================================================

#define BTN_DEL_X              5
#define BTN_DEL_Y              56
#define BTN_DEL_W              55
#define BTN_DEL_H              0

// ============================================================
// CONTROLES
// ============================================================

#define BTN_DELETE_X           5
#define BTN_DELETE_Y           8
#define BTN_DELETE_W           48
#define BTN_DELETE_H           40

#define BTN_LEFT_X             57
#define BTN_LEFT_Y             8
#define BTN_LEFT_W             48
#define BTN_LEFT_H             40

#define BTN_RIGHT_X            109
#define BTN_RIGHT_Y            8
#define BTN_RIGHT_W            48
#define BTN_RIGHT_H            40

#define BTN_OK_X               161
#define BTN_OK_Y               8
#define BTN_OK_W               74
#define BTN_OK_H               40

// ============================================================
// AFFICHAGE
// ============================================================

#define KEYBOARD_TEXT_SIZE     2
#define KEYBOARD_BORDER        1

#endif