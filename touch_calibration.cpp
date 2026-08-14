// ============================================================
// UPDATE AUTOMATIQUE CALIBRATION
// ============================================================
//
// Cette fonction est appelee depuis loop().
//
// Elle lit le tactile avec getTouch() puis transmet les
// coordonnees a touchCalibrationUpdate().
//
// IMPORTANT :
// getTouchRaw() ne doit pas etre utilise comme detection
// de toucher. getTouch() gere la pression et retourne
// correctement l'etat tactile.
// ============================================================

void updateTouchCalibration()
{
    uint16_t x = 0;
    uint16_t y = 0;

    // --------------------------------------------------------
    // Lecture tactile
    // --------------------------------------------------------

    if (
        !tft.getTouch(
            &x,
            &y,
            TOUCH_THRESHOLD
        )
    )
    {
        return;
    }

    // --------------------------------------------------------
    // Anti-rebond
    // --------------------------------------------------------

    uint32_t now =
        millis();

    if (
        now -
        lastTouchTime
        <
        TOUCH_DEBOUNCE_TIME
    )
    {
        return;
    }

    lastTouchTime =
        now;

    // --------------------------------------------------------
    // Debug
    // --------------------------------------------------------

    Serial.print(
        "[TOUCH] X="
    );

    Serial.print(
        x
    );

    Serial.print(
        " Y="
    );

    Serial.println(
        y
    );

    // --------------------------------------------------------
    // Traitement
    // --------------------------------------------------------

    touchCalibrationUpdate(
        x,
        y
    );

    // --------------------------------------------------------
    // Attendre le relachement du doigt
    // --------------------------------------------------------

    while (
        tft.getTouch(
            &x,
            &y,
            TOUCH_THRESHOLD
        )
    )
    {
        delay(
            10
        );

        yield();
    }
}