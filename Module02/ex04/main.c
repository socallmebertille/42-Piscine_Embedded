#include "main.h"

int main(void)
{
    cli();
    uart_init();          // Configure l'UART
    led_init();           // Configure les pins en sortie
    timer_led_init();     // Démarre le timer pour l'effet de chargement
    sei();

    char step = 1;
    while (1)
    {
        if (step == 1)
        {
            uart_printstr("Enter your login:\r\n");
            char res = get_input(1) & get_input(0);
            // led_blink_dramatic();
            if (res)
            {
                uart_printstr("Hello saberton!\n\r");
                uart_printstr("Shall we play a game?\r\n");
                step++;
            }
            else
                uart_printstr("Bad combination username/password\r\n");
            res_led_blink(res);
        }
    }
}
