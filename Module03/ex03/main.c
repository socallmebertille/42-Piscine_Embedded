#include "main.h"

int main(void)
{
    cli();                // Ignorer les interruptions
    uart_init();          // Configure l'UART
    led_init();           // Configure LEDs
    sei();                // Autoriser les interruptions

    while (1)
    {
        get_color();      // Récupère la couleur et l'affiche
        _delay_ms(10);    // Léger temps de repos puis re boucle
    }
}