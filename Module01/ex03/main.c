#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= (1 << PB1);                   // LED D2 en sortie
    DDRD &= ~((1 << PD2) | (1 << PD4));   // SW1 et SW2 boutons en entree
    PORTD |= (1 << PD2) | (1 << PD4);     // Active pull-up pour SW1 et SW2

    // Configuration Timer1 : Fast PWM mode 14, prescaler /1024
    TCCR1A = (1 << COM1A1) | (1 << WGM11);                             // Clear OC1A on match, WGM11
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);  // WGM13:12, prescaler /1024

    ICR1 = 15624;                        // TOP pour 1 Hz = 16 MHz / 1024
    uint16_t step = ICR1 / 10;           // 10% du cycle
    OCR1A = step;                        // Démarre à 10% duty cycle

    while (1)
    {
        if (!(PIND & (1 << PD2)))              // Si on appuie sur SW1
        {
            _delay_ms(50);                     // Anti-rebond
            if (!(PIND & (1 << PD2)))          // Toujours appuyé après anti-rebond ?
            {
                if (OCR1A <= ICR1 - step)
                    OCR1A += step;             // on augmente le duty cyle de 10%
                else
                    OCR1A = ICR1;              // Plafonner à 100%
                while (!(PIND & (1 << PD2)));  // Attendre relâchement
                _delay_ms(50);
            }
        }
        if (!(PIND & (1 << PD4)))              // Si on appuie du SW2
        {
            _delay_ms(50);                     // Anti-rebond
            if (!(PIND & (1 << PD4)))          // Toujours appuyé après anti-rebond ?
            {
                if (OCR1A > step)              // Si on peut encore décrémenter (min 10%)
                    OCR1A -= step;             // on réduit le duty cycle de 10%

                while (!(PIND & (1 << PD4)));  // Attendre relâchement
                _delay_ms(50); 
            }
        }
    }
}
