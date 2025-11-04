#include <avr/io.h>
#include <util/delay.h>

#define LED_R  PD5
#define LED_G  PD6
#define LED_B  PD3

int main(void)
{
    DDRD |= (1 << LED_R) | (1 << LED_G) | (1 << LED_B); // LEDs RGB en sortie
    unsigned char step = 0;

    while (1)
    {
        PORTD &= ~((1 << LED_R) | (1 << LED_G) | (1 << LED_B)); // Éteint uniquement les LED RGB

        switch (step)
        {
            case 0:
                PORTD |= (1 << LED_R); // Allume Rouge
                break;
            case 1:
                PORTD |= (1 << LED_G); // Allume Vert
                break;
            case 2:
                PORTD |= (1 << LED_B); // Allume Bleu
                break;
        }

        step = (step + 1) % 3; // Step suivant
        _delay_ms(1000); // Attend 1 sec
    }
}
