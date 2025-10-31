#include <avr/io.h>

int main(void)
{
    int pressed = 0;
    DDRB |= (1 << PB0); // Configuration LED (sortie)
    DDRD &= ~(1 << PD2); // PD2 (= boutton) en entrée
    PORTD |= (1 << PD2); // Active le pull-up sur PD2

    while (1)
    {
        if (!pressed && !(PIND & (1 << PD2))) // Bouton appuyé (0) et que juste avant il était relaché
        {
            pressed = 1;
            PORTB |= (1 << PB0); // LED ON
        }
        else if (pressed && (PIND & (1 << PD2))) // Bouton relâché (1) et que juste avant il était enfoncé
        {
            pressed = 0;
            PORTB &= ~(1 << PB0); // LED OFF
        }
    }

    return 0;
}
