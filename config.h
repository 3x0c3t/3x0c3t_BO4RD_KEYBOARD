#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

// ============================================================
// 3x0c3t BO4RD KEYBOARD v1.0
// Configuration
// ============================================================


// ------------------------------------------------------------
// Écran
// ------------------------------------------------------------

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define SCREEN_ROTATION 2


// ------------------------------------------------------------
// Calibration tactile
//
// 1 = calibration à chaque démarrage
// 0 = pas de calibration
// ------------------------------------------------------------

#define TOUCH_CALIBRATION 1


// ------------------------------------------------------------
// Anciennes valeurs de calibration
//
// Ces valeurs servent lorsque l'utilisateur choisit
// "reprendre les valeurs précédentes".
//
// Dernières valeurs relevées :
// X min = 291
// X max = 3387
// Y min = 410
// Y max = 3387 environ
//
// À ajuster si nécessaire après une nouvelle calibration.
// ------------------------------------------------------------

#define TOUCH_PREVIOUS_CAL_0 291
#define TOUCH_PREVIOUS_CAL_1 3387
#define TOUCH_PREVIOUS_CAL_2 410
#define TOUCH_PREVIOUS_CAL_3 3387
#define TOUCH_PREVIOUS_CAL_4 2


// ------------------------------------------------------------
// Zone du bouton permettant de reprendre la calibration
// précédente
//
// Coordonnées écran, rotation 2
// ------------------------------------------------------------

#define TOUCH_REUSE_X 20
#define TOUCH_REUSE_Y 235
#define TOUCH_REUSE_W 200
#define TOUCH_REUSE_H 35


// ------------------------------------------------------------
// Zone clavier
// ------------------------------------------------------------

#define KEYBOARD_X 0
#define KEYBOARD_Y 138
#define KEYBOARD_W 240
#define KEYBOARD_H 182

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10


// ------------------------------------------------------------
// Champ de saisie
// ------------------------------------------------------------

#define INPUT_X 5
#define INPUT_Y 5
#define INPUT_W 170
#define INPUT_H 40


// ------------------------------------------------------------
// Bouton DEL
// ------------------------------------------------------------

#define BTN_DEL_X 180
#define BTN_DEL_Y 5
#define BTN_DEL_W 55
#define BTN_DEL_H 40


// ------------------------------------------------------------
// Bouton OK
// ------------------------------------------------------------

#define BTN_OK_X 180
#define BTN_OK_Y 50
#define BTN_OK_W 55
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