#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    int on = 0;
    DDRB |= (1 << PB0);
    DDRD &= ~(1 << PD2);
    
    while (1)
    {
        if (!(PIND & (1 << PD2))) // si on clique sur le boutton
        {
            _delay_ms(50); // on attend un chouille
            if (PIND & (1 << PD2)) // si on a finit de cliqué
            {
                if (!on) // si c'était éteint on allume
                {
                    on = 1;
                    PORTB |= (1 << PB0);
                }
                else // si c'était allumé on éteint
                {
                    on = 0;
                    PORTB &= ~(1 << PB0);
                }
            }
        }
    }

    return 0;
}
