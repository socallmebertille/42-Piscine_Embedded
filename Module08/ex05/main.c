#include "main.h"

int main(void)
{
    uart_init();
    adc_init();
    SPI_MasterInit();

    

    while (1)
    {
        
        

        send_action_frame(0x00);                   // start frame

        for (uint8_t j = 0; j < 3; j++)
        {
            led_send_frame(0, 0, 0, 0);
        }

        send_action_frame(0xFF);                   // end frame
    }
}
