#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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
            case 0: // Rouge
                PORTD |= (1 << LED_R);
                break;
            case 1: // Vert
                PORTD |= (1 << LED_G);
                break;
            case 2: // Bleu
                PORTD |= (1 << LED_B);
                break;
            case 3: // Jaune (Vert + Rouge)
                PORTD |= (1 << LED_G) | (1 << LED_R);
                break;
            case 4: // Cyan (Vert + Bleu)
                PORTD |= (1 << LED_G) | (1 << LED_B);
                break;
            case 5: // Magenta (Rouge + Bleu)
                PORTD |= (1 << LED_R) | (1 << LED_B);
                break;
            case 6: // Blanc (toutes les couleurs)
                PORTD |= (1 << LED_G) | (1 << LED_R) | (1 << LED_B);
                break;
        }
        step = (step + 1) % 7;
        _delay_ms(1000); // Attend 1 sec
    }
}
