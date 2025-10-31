#include <avr/io.h>

int main(void)
{
    // initialisation
    DDRB |= (1 << PB1);   // LED en mode sortie
    PORTB &= ~(1 << PB1); // LED OFF

    return 0;
}
