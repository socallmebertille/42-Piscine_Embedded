#include "main.h"

int main(void)
{
    SPI_MasterInit();

    // Tableau de couleurs (R, G, B)
    const uint8_t colors[][3] = {
        {0xFF, 0x00, 0x00}, // red
        {0x00, 0xFF, 0x00}, // green
        {0x00, 0x00, 0xFF}, // blue
        {0xFF, 0xFF, 0x00}, // yellow
        {0x00, 0xFF, 0xFF}, // cyan
        {0xFF, 0x00, 0xFF}, // magenta
        {0xFF, 0xFF, 0xFF}, // white
    };

    const uint8_t nb_colors = sizeof(colors) / sizeof(colors[0]);
    uint8_t i = 0;

    while (1)
    {
        for (uint8_t led = 0; led < 4; led++)
        {
            send_action_frame(0x00);

            for (uint8_t j = 0; j < 3; j++)
            {
                if (j == led && led < 3)
                    led_send_frame(colors[i][0], colors[i][1], colors[i][2], 1);
                else
                    led_send_frame(0, 0, 0, 0);
            }

            send_action_frame(0xFF);

            _delay_ms(250);

            i++;
            if (i >= nb_colors) i = 0;
        }
    }
}
