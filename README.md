# 3x0c3t BO4RD KEYBOARD v1.0

Clavier tactile pour TFT 240x320 avec ESP8266,
TFT_eSPI et XPT2046.

## Materiel

ESP8266 NodeMCU

TFT ILI9341 240x320

Touchscreen XPT2046

## Rotation

Rotation TFT_eSPI :

2

Resolution logique :

240 x 320

## Architecture

TFT_eSPI gere :

- TFT
- SPI
- XPT2046
- lecture tactile
- calibration
- conversion des coordonnées

Le module keyboard reçoit uniquement :

X
Y

## Touch

La lecture tactile utilise :

tft.getTouch()

Aucune lecture SPI XPT2046 manuelle n'est utilisée.

## Clavier

Mode alphabetique :

QWERTYUIOP
ASDFGHJKL
ZXCVBNM
1234567890

Mode numerique :

1234567890
-+*/=()<>
.,:;!?%#
ABCDEFGHIJ

## Commandes

ABC / 123 :
changement de clavier

< :
retour arrière

OK :
validation du texte

## Fichiers

KEYBOARD.ino
config.h
keyboard.h
keyboard.cpp
touch_calibration.h
touch_calibration.cpp
README.md