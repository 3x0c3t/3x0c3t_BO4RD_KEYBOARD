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
#define KEYBOARD_TOUCH_DELAY 120

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
// Zone de saisie :
//     5 .. 64
//
// Clavier :
//     75 .. 155
//
// 4 lignes de 20 pixels exactement.
//
// Ligne 1 :
//     A B C D E F G H I SYM
//
// Ligne 2 :
//     J K L M N O P Q R
//
// Ligne 3 :
//     S T U V W X Y Z
//
// Ligne 4 :
//     0 1 2 3 4 5 6 7 8 9
//
// ============================================================

#define KEYBOARD_X             0
#define KEYBOARD_Y             75

#define KEYBOARD_W             SCREEN_WIDTH
#define KEYBOARD_HEIGHT        80

#define KEYBOARD_ROWS          4
#define KEYBOARD_COLS          10

#define KEYBOARD_KEY_HEIGHT    20

#define KEYBOARD_MAX_LENGTH    64

// ============================================================
// ZONE DE SAISIE
// ============================================================

#define INPUT_X                5
#define INPUT_Y                5
#define INPUT_W                230
#define INPUT_H                60

// ============================================================
// ZONE TEXTE
// ============================================================
//
// La partie gauche est réservée au texte.
// Les commandes sont à droite.
//

#define INPUT_TEXT_X           8
#define INPUT_TEXT_Y           20
#define INPUT_TEXT_W           138
#define INPUT_TEXT_H           35

// ============================================================
// CONTROLES DANS LA ZONE DE SAISIE
// ============================================================
//
// À droite :
//
//     DEL | < | > | OK
//
// ============================================================

#define BTN_DELETE_X           150
#define BTN_DELETE_Y           20
#define BTN_DELETE_W           19
#define BTN_DELETE_H           30

#define BTN_LEFT_X             170
#define BTN_LEFT_Y             20
#define BTN_LEFT_W             19
#define BTN_LEFT_H             30

#define BTN_RIGHT_X            190
#define BTN_RIGHT_Y            20
#define BTN_RIGHT_W             19
#define BTN_RIGHT_H             30

#define BTN_OK_X               210
#define BTN_OK_Y               20
#define BTN_OK_W               20
#define BTN_OK_H               30

// ============================================================
// AFFICHAGE
// ============================================================

#define KEYBOARD_TEXT_SIZE     1
#define KEYBOARD_BORDER        1

#endif