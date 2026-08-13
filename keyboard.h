#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>

// Initialise le clavier
void keyboardBegin();

// Dessine le clavier
void keyboardDraw();

// Traite un appui tactile
void keyboardUpdate(int16_t x, int16_t y);

// Retourne le texte saisi
const char* keyboardGetText();

// Efface le texte
void keyboardClearText();

// Indique si la touche VALIDER a été pressée
bool keyboardWasValidated();

// Efface l'état VALIDÉ
void keyboardClearValidated();

#endif