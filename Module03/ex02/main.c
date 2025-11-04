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
            wheel(i);      // Intensité des différentes 3 couleurs pour parcourir toutes les combinaisons
            _delay_ms(10); // ATTEND 10ms avant la prochaine couleur -> Vitesse de transition
        }
    }
}

/*

Choix du prescaler selon rapidité de clignotement visible à l'oeil nu

| Prescaler	| Formule	         | F_PWM    |	Perception               |
|-----------|--------------------|----------|----------------------------|
| /1	    | 16M / (1 × 256)	 | 62.5 kHz	| Trop rapide, gaspille CPU  |
| /8	    | 16M / (8 × 256)	 | 7.8 kHz	| Rapide, bon pour servo     |
| /64 ⭐	   | 16M / (64 × 256)	| 976 Hz   | Parfait pour LED ✓         |
| /256	    | 16M / (256 × 256)	 | 244 Hz	| Visible, clignotement      |
| /1024     | 16M / (1024 × 256) |	61 Hz	| Trop lent, scintillement   |

Choix du temps de délais avant chaque changement de couleurs

| Délai	    |Temps pour 1 cycle complet |	Effet visuel                                  |
|-----------|---------------------------|-------------------------------------------------|
| 0 ms	    | 0 secondes	            | Trop rapide, couleurs floues, impossible à voir |
| 1 ms	    | 256 ms = 0.25 sec	        | Très rapide, arc-en-ciel fluide mais rapide     |
| 10 ms ⭐  | 2560 ms = 2.5 sec	       | Fluide et visible ✓                             |
| 50 ms	    | 12.8 sec	                | Lent, transitions visibles                      |
| 100 ms    | 25.6 sec	                | Très lent, défilement saccadé                   |

*/