#include "touch_calibration.h"
#include "config.h"

#include <SPI.h>
#include <XPT2046_Touchscreen.h>

/*
 * XPT2046
 *
 * SPI partagé avec le TFT.
 *
 * ESP8266 :
 * SCK  = GPIO14 / D5
 * MISO = GPIO12 / D6
 * MOSI = GPIO13 / D7
 *
 * Touch CS  = GPIO0 / D3
 * Touch IRQ = GPIO5 / D1
 */

#ifndef TOUCH_CS
#define TOUCH_CS 0
#endif

#ifndef TOUCH_IRQ
#define TOUCH_IRQ 5
#endif

static XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

static bool touchInitialized = false;

/*
 * Calibration tactile.
 *
 * Ces valeurs doivent correspondre à la calibration
 * fonctionnelle utilisée précédemment sur ton écran.
 *
 * Si ta calibration actuelle utilise d'autres valeurs,
 * elles peuvent être remplacées ici.
 */
#ifndef TOUCH_MIN_X
#define TOUCH_MIN_X 200
#endif

#ifndef TOUCH_MAX_X
#define TOUCH_MAX_X 3900
#endif

#ifndef TOUCH_MIN_Y
#define TOUCH_MIN_Y 200
#endif

#ifndef TOUCH_MAX_Y
#define TOUCH_MAX_Y 3900
#endif

void touchCalibrationBegin()
{
    Serial.println("[TOUCH] Init");

    /*
     * ESP8266 :
     * SPI.begin() ne prend aucun argument.
     *
     * Le brochage SPI matériel est fixe :
     * SCK  GPIO14
     * MISO GPIO12
     * MOSI GPIO13
     */
    SPI.begin();

    delay(50);

    if (touch.begin())
    {
        touchInitialized = true;

        /*
         * Le clavier utilise une rotation 2 :
         * écran logique 240 x 320.
         */
        touch.setRotation(2);

        Serial.println("[TOUCH] OK");
    }
    else
    {
        touchInitialized = false;
        Serial.println("[TOUCH] ERREUR");
    }
}

/*
 * Lecture tactile et conversion vers les coordonnées écran.
 *
 * Retourne :
 *   true  = contact détecté
 *   false = aucun contact
 *
 * x et y sont des pointeurs afin que la fonction puisse
 * retourner les coordonnées calculées.
 */
bool touchReadScreen(int16_t* x, int16_t* y)
{
    if (x == nullptr || y == nullptr)
    {
        return false;
    }

    *x = -1;
    *y = -1;

    if (!touchInitialized)
    {
        return false;
    }

    if (!touch.touched())
    {
        return false;
    }

    TS_Point p = touch.getPoint();

    /*
     * Valeurs brutes XPT2046.
     */
    int32_t rawX = p.x;
    int32_t rawY = p.y;

    /*
     * Protection contre les valeurs manifestement invalides.
     */
    if (rawX < 0 || rawX > 4095 ||
        rawY < 0 || rawY > 4095)
    {
        return false;
    }

    /*
     * Conversion brute -> écran.
     *
     * Rotation 2.
     *
     * On utilise map() plutôt qu'une division manuelle
     * afin de conserver une conversion entière propre.
     */
    int32_t screenX = map(
        rawX,
        TOUCH_MIN_X,
        TOUCH_MAX_X,
        239,
        0
    );

    int32_t screenY = map(
        rawY,
        TOUCH_MIN_Y,
        TOUCH_MAX_Y,
        319,
        0
    );

    /*
     * Limitation aux dimensions réelles du TFT.
     */
    screenX = constrain(screenX, 0, 239);
    screenY = constrain(screenY, 0, 319);

    *x = (int16_t)screenX;
    *y = (int16_t)screenY;

    return true;
}