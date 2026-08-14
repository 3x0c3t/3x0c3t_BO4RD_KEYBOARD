#ifndef BO4RD_KEYBOARD_CONFIG_H
#define BO4RD_KEYBOARD_CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// ECRAN TFT
// ============================================================
//
// UNE SEULE SOURCE DE VERITE POUR LA RESOLUTION
//
// ILI9341
// Portrait
// Rotation TFT_eSPI = 2
//
// ============================================================

#define SCREEN_WIDTH       240
#define SCREEN_HEIGHT      320
#define SCREEN_ROTATION    2

// ============================================================
// TOUCH
// ============================================================

#define TOUCH_THRESHOLD    600

// ============================================================
// EEPROM - CALIBRATION TACTILE
// ============================================================

#define TOUCH_EEPROM_SIZE       64
#define TOUCH_EEPROM_MAGIC      0x3C7A
#define TOUCH_EEPROM_VERSION    1
#define TOUCH_EEPROM_ADDRESS    0

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

#define KEYBOARD_X            0
#define KEYBOARD_Y            58

#define KEYBOARD_W            SCREEN_WIDTH
#define KEYBOARD_HEIGHT       208

#define KEYBOARD_ROWS         4
#define KEYBOARD_COLS         10

#define KEYBOARD_MAX_LENGTH   32

// ============================================================
// ZONE TEXTE
// ============================================================

#define INPUT_X               5
#define INPUT_Y               5
#define INPUT_W               165
#define INPUT_H               42

// ============================================================
// BOUTON DELETE
// ============================================================

#define BTN_DEL_X             175
#define BTN_DEL_Y             5
#define BTN_DEL_W             60
#define BTN_DEL_H             42

// ============================================================
// BOUTON OK
// ============================================================

#define BTN_OK_X              175
#define BTN_OK_Y              273
#define BTN_OK_W              60
#define BTN_OK_H              42

// ============================================================
// AFFICHAGE
// ============================================================

#define KEYBOARD_TEXT_SIZE    2
#define KEYBOARD_BORDER       2

#endif