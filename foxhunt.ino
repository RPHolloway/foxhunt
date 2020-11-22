#include "EEPROM.h"

#include <coop_config.h>
#include <coop_threads.h>

#include "display.h"

#define GPIO_BUTTON (2)
#define ANTENNA_SPIN_RATE_HZ (600)

uint8_t LedOffset = 0;

typedef enum
{
    buttonState_Pressed = 0,
    buttonState_Released = 1,
    buttonState_Held = 2
} buttonState_t;

/*
 * "Spin" the antenna 
 */
extern "C" void service_antennas(void *arg)
{
    static int antenna = 0;

    while (true)
    {
        coop_idle(1667);
    }
}

buttonState_t buttonState = buttonState_Released;
void button_change()
{
    delay(10);
    buttonState = (buttonState_t)digitalRead(GPIO_BUTTON);
    
    // Notify the button handler of the state change
    coop_notify(0);
}

extern "C" void service_button(void *arg)
{
    while (true)
    {
        const int holdDelay = 500;
        static unsigned long lastButtonEvent = 0;
        static bool inCalibrationMode = false;
        
        // Check if the button has been held for longer than
        // the hold delay.
        if (buttonState == buttonState_Released 
            && (millis() - lastButtonEvent) > holdDelay)
        {
            if (!inCalibrationMode)
            {
                Display_EnterCalibration(LedOffset);
                inCalibrationMode = true;
            }
            else
            {
                // Save LED origin
                EEPROM.put(0, LedOffset);
                Display_ExitCalibration(LedOffset);
                inCalibrationMode = false;
            }
        }
        else if (buttonState == buttonState_Released)
        {
            if (inCalibrationMode)
            {
                LedOffset = (LedOffset + 1) % 16;
                Display_SetDirection(LedOffset);
            }
        }

        lastButtonEvent = millis();
        coop_wait(0, 0);
    }
}

void setup()
{
    // Initalize the display
    EEPROM.get(0, LedOffset);
    Display_Init(LedOffset);

    // Attach button
    pinMode(GPIO_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON), button_change, CHANGE);

    coop_sched_thread(service_antennas, "antennas", 0, (void *)NULL);
    coop_sched_thread(service_button, "button", 0, (void *)NULL);
    coop_sched_service();
}

void loop() { }
