#include "main.h"

char *read_input()
{

}

int main(void)
{
    uart_init();
    SPI_MasterInit();

    char buffer[8] = "";
    char rainbow[11] = "FULLRAINBOW";

    while (1)
    {
        
        uint8_t leds_on = 0;

        if (buffer[7] == "6") leds_on = 3;      // D6, D7, D8
        else if (buffer[7] == "7") leds_on = 2; // D6, D7
        else if (buffer[7] == "8") leds_on = 1; // D6
        else leds_on = 0;                       // aucune LED

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
