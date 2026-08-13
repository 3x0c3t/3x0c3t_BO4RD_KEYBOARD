#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================


// ------------------------------------------------------------
// Écran
// ------------------------------------------------------------

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  320

#define SCREEN_ROTATION 2


// ------------------------------------------------------------
// Calibration tactile
//
// 1 = calibration à chaque démarrage
// 0 = pas de calibration
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Calibration : bouton reprise configuration précédente
// ------------------------------------------------------------

#define TOUCH_REUSE_X 20
#define TOUCH_REUSE_Y 105
#define TOUCH_REUSE_W 200
#define TOUCH_REUSE_H 42


// ------------------------------------------------------------
// Calibration : positions des cibles
// ------------------------------------------------------------

// Cible haut gauche
#define TOUCH_TARGET_1_X 20
#define TOUCH_TARGET_1_Y 20

// Cible haut droite
#define TOUCH_TARGET_2_X 220
#define TOUCH_TARGET_2_Y 20

// Cible bas gauche
#define TOUCH_TARGET_3_X 20
#define TOUCH_TARGET_3_Y 300

// Cible bas droite
#define TOUCH_TARGET_4_X 220
#define TOUCH_TARGET_4_Y 300


// ------------------------------------------------------------
// Calibration : apparence des cibles
// ------------------------------------------------------------

#define TOUCH_TARGET_RADIUS 10
#define TOUCH_TARGET_CROSS  6


// ------------------------------------------------------------
// Anciennes valeurs de calibration
//
// Dernières valeurs fonctionnelles connues
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0 294
#define TOUCH_PREVIOUS_CAL_1 3385
#define TOUCH_PREVIOUS_CAL_2 442
#define TOUCH_PREVIOUS_CAL_3 328
#define TOUCH_PREVIOUS_CAL_4 3380


// ------------------------------------------------------------
// Zone clavier
// ------------------------------------------------------------

#define KEYBOARD_X 0
#define KEYBOARD_Y 138
#define KEYBOARD_W 240
#define KEYBOARD_H 182


// ------------------------------------------------------------
// Clavier
// ------------------------------------------------------------

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10


// ------------------------------------------------------------
// Champ de saisie
// ------------------------------------------------------------

#define INPUT_X 5
#define INPUT_Y 92
#define INPUT_W 230
#define INPUT_H 40


// ------------------------------------------------------------
// Bouton DEL
// ------------------------------------------------------------

#define BTN_DEL_X 5
#define BTN_DEL_Y 45
#define BTN_DEL_W 110
#define BTN_DEL_H 40


// ------------------------------------------------------------
// Bouton OK
// ------------------------------------------------------------

#define BTN_OK_X 120
#define BTN_OK_Y 45
#define BTN_OK_W 115
#define BTN_OK_H 40


// ------------------------------------------------------------
// Couleurs
// ------------------------------------------------------------

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


// ------------------------------------------------------------
// Texte
// ------------------------------------------------------------

#define KEYBOARD_MAX_TEXT 64


#endif