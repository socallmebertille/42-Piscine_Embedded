#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_rgb(void)
{
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3); // LEDs RGB en sortie

    // Initialisation Timer0 car broche PD5 et PD6 reliées a OC0A:B
    TCCR0A |= (1 << WGM01) | (1 << WGM00);   // Mode Fast PWM
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // PWM sur OC0A (PD6) et OC0B (PD5)

    TCCR0B |= (1 << CS01) | (1 << CS00);     // Prescaler /64

    // Initialisation Timer2 car broche PD3 reliee a OC2B
    TCCR2A |= (1 << WGM21) | (1 << WGM20);   // Mode Fast PWM
    TCCR2A |= (1 << COM2B1);                 // PWM sur OC2B (PD3)

    TCCR2B |= (1 << CS22);                   // Prescaler /64
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; // Rouge (PD5)
    OCR0A = g; // Vert (PD6)
    OCR2B = b; // Bleu (PD3)
}

void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

int main(void)
{
    init_rgb();

    while (1)
    {
        for (uint8_t i = 0; i < 255; i++)
        {
            wheel(i);
            _delay_ms(10); // Vitesse de transition
        }
    }
}
