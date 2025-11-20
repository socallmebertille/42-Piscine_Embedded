#include "main.h"

int main(void)
{
    adc_init();
    SPI_MasterInit();

    const uint8_t threshold_33 = 255 * 33 / 100;   // 84
    const uint8_t threshold_66 = 255 * 66 / 100;   // 168
    const uint8_t threshold_100 = 255;             // 255

    while (1)
    {
        uint8_t val = adc_read(0);
        uint8_t leds_on = 0;

        if (val >= threshold_100) leds_on = 3;     // D6, D7, D8
        else if (val >= threshold_66) leds_on = 2; // D6, D7
        else if (val >= threshold_33) leds_on = 1; // D6
        else leds_on = 0;                          // aucune LED

        send_action_frame(0x00);                   // start frame

        for (uint8_t j = 0; j < 3; j++)
        {
            if (j < leds_on)
                led_send_frame(0xFF, 0xFF, 255, 1);// blanc et faible luminosité
            else
                led_send_frame(0, 0, 0, 0);
        }

        send_action_frame(0xFF);                   // end frame
    }
}
