#include <avr/io.h>
#include <util/delay.h>

void turn_my_led(char value)
{
    PORTB = 0;                // j'éteins mes 4 leds
    if (value & (1 << 0))     // si value_bit[0] == 1
        PORTB |= (1 << PB0);  // j'allime ma led D1
    if (value & (1 << 1))     // si value_bit[1] == 1
        PORTB |= (1 << PB1);  // j'allume ma led D2
    if (value & (1 << 2))     // si value_bit[2] == 1
        PORTB |= (1 << PB2);  // j'allume ma led D3
    if (value & (1 << 3))     // si value_bit[3] == 1
        PORTB |= (1 << PB4);  // j'allume ma led D4
}

int main(void)
{
    // initialisation à 0
    char value = 0;
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
    // initialisation des boutons comme des entrées
    DDRD &= ~(1 << PD2);
    DDRD &= ~(1 << PD4);

    while (1)
    {
        turn_my_led(value); // j'allume mes leds en fonction des bits dans la value
        if (!(PIND & (1 << PD2))) // bouton SW1 appuyé
        {
            _delay_ms(50);
            if (PIND & (1 << PD2)) // bouton SW2 laché
                value++; // incrémentation
        }
        else if (!(PIND & (1 << PD4))) // bouton SW2 appuyé
        {
            _delay_ms(50);
            if (PIND & (1 << PD4)) // bouton SW2 relaché
                value--; // décrémentation
        }
    }

    return 0;
}
