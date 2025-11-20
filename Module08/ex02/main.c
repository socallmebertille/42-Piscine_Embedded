#include "main.h"

int main(void)
{
    SPI_MasterInit();

    
    while (1)
    {
        send_action_frame(0x00);            // Start frame = 4 × 00 byte
    
        led_send_frame(0xFF, 0, 0, 0b00001); // LED 1 = D6 → Rouge
        led_send_frame(0, 0, 0, 0b00000);   // LED 2 = D7 → OFF
        led_send_frame(0, 0, 0, 0b00000);   // LED 3 = D8 → OFF
    
        send_action_frame(0xFF);            // End frame =  4 x FF byte
    }
}

/*

datasheet LEDs APA102 :
https://www.mouser.com/datasheet/2/737/APA102_2020_SMD_LED-2487271.pdf?srsltid=AfmBOoo6ZRvUoNqNxSZqwsKGs73r1bb3UH-bg7t5GAkawnmmO5ukuRcV

p.3 | cascading data structure 
-> SDI = « Serial Data Input » = la ligne de données
=> procotocole de transmission de données en série

| Frame send  | 0:7 bits     | 8:15      | 16:23     | 24:31     |
| ------------|--------------|-----------|-----------|-----------|
| START Frame | 0000 0000    | 0000 0000 | 0000 0000 | 0000 0000 |
| LED_1 Frame | ...          |           |           |           |
| LED_i Frame | 111 | global | blue      | green     | red       |
| LED_n Frame | ...          |           |           |           |
| END Frame   | 1111 1111    | 1111 1111 | 1111 1111 | 1111 1111 |

*/