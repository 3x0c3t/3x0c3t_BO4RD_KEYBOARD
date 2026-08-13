# 3x0c3t BO4RD KEYPAD

Keypad tactile pour ESP8266 avec écran TFT SPI 2.8" 240x320 basé sur ILI9341 et contrôleur tactile XPT2046.

Le projet utilise la bibliothèque TFT_eSPI et est structuré en plusieurs fichiers afin de séparer la configuration, l'affichage, le clavier et la calibration tactile.

## Version

v1.0

## État du projet

Projet fonctionnel.

- ESP8266 : OK
- TFT ILI9341 : OK
- Résolution : 240x320
- Orientation : portrait
- Rotation TFT_eSPI : 2
- Touch XPT2046 : OK
- Calibration tactile : OK
- Sauvegarde calibration : OK
- SPIFFS : OK
- Keypad : OK
- Détection des boutons : OK
- Affichage du buffer : OK
- Entrée numérique : OK
- Suppression : OK
- Envoi série : OK
- Logs de diagnostic : OK

---

# 1. Présentation

Le projet `3x0c3t_BO4RD_KEYPAD` est un clavier numérique tactile destiné à être utilisé avec une carte ESP8266 équipée d'un écran TFT SPI 2.8".

L'interface permet de saisir une valeur à l'aide de 15 boutons tactiles.

Les boutons disponibles sont :

- New
- Del
- Send
- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8
- 9
- .
- 0
- #

La valeur saisie est affichée dans une zone supérieure de l'écran.

Le bouton `Send` transmet la valeur au moniteur série.

---

# 2. Matériel

## Microcontrôleur

ESP8266.

Le projet a été développé et testé avec l'environnement Arduino.

## Écran

TFT SPI 2.8".

Contrôleur :

ILI9341

Résolution :

240 x 320 pixels

Orientation utilisée :

Portrait

Rotation TFT_eSPI :

2

## Contrôleur tactile

XPT2046.

Le tactile utilise le même bus SPI que le TFT avec une broche CS dédiée.

---

# 3. Bibliothèques

Le projet utilise principalement :

```text
TFT_eSPI
SPI
FS