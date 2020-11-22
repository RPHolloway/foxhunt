/**
 * Initialize the display to blank
 */
void Display_Init(uint8_t origin);

/**
 * Incrementally lights up all of the LEDs to indicate
 * the board is entering calibration mode.
 */
void Display_EnterCalibration(uint8_t origin);

/**
 * Blink the choosen zero LCD and change the color to indicate
 * calibration is complete.
 */
void Display_ExitCalibration(uint8_t origin);

/**
 * Turns on the LED in the commanded direction.
 * 
 * @param pixel LED to turn on. All other LEDs off.
 */
void Display_SetDirection(uint8_t pixel);