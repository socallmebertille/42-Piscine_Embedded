#include "main.h"

void led_init(void)
{
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4); // Active mes LEDs d'attente en mode sortie
    PORTB = 0; // Éteint mes LEDs d'attente
}

void timer_led_init(void)
{
    // Mode CTC (Clear Timer on Compare Match)
    TCCR0A |= (1 << WGM01);
    
    // Prescaler = /1 (plus rapide pour PWM)
    TCCR0B |= (1 << CS10);
    
    // Fréquence PWM ~1kHz ---> 16MHz / 64 / 250 = 1kHz
    OCR0A = 250;
    
    // Active l'interrupt sur comparaison
    TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
    led_loading();
}

void led_loading(void)
{
    static unsigned char brightness[4] = {255, 128, 64, 16};  // Intensités différentes
    static unsigned char counter = 0;  // Compteur PWM (0-255)
    static unsigned char shift_counter = 0;
    
    counter++;
    
    // Software PWM : allume la LED si brightness > counter
    PORTB = 0;
    if (brightness[0] > counter) PORTB |= (1 << PB0);
    if (brightness[1] > counter) PORTB |= (1 << PB1);
    if (brightness[2] > counter) PORTB |= (1 << PB2);
    if (brightness[3] > counter) PORTB |= (1 << PB4);
    
    // Toutes les 256 itérations, décale le dégradé
    if (counter == 0) {
        shift_counter++;
        if (shift_counter >= 20) {  // Vitesse de rotation (ajustez)
            shift_counter = 0;
            // Rotation du dégradé
            unsigned char temp = brightness[0];
            brightness[0] = brightness[1];
            brightness[1] = brightness[2];
            brightness[2] = brightness[3];
            brightness[3] = temp;
        }
    }
}

void res_led_blink(char res)
{
    // Arrête le Timer0 (effet de chargement)
    TIMSK0 &= ~(1 << OCIE0A);

    PORTB = 0; // Éteint mes LEDs d'attente

    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6); // Active ma LED RGB en mode sortie
    PORTD = 0; // Éteint ma LED RGB

    if (res == 1) // Arc-en-ciel / discothèque
    {
        for (char i = 0; i < 3; i++)
        {
            PORTD |= (1 << PD6);  // Vert ON (PD6)
            _delay_ms(200);
            PORTD = 0;            // RGB OFF
            _delay_ms(200);
        }
        // Effet disco : 20 clignotements de couleurs aléatoires
        for (char i = 0; i < 20; i++)
        {
            // Cycle de couleurs : Rouge → Vert → Bleu → Jaune → Cyan → Magenta → Blanc
            switch (i % 7)
            {
                case 0: // Rouge
                    PORTD = (1 << PD6);
                    break;
                case 1: // Vert
                    PORTD = (1 << PD5);
                    break;
                case 2: // Bleu
                    PORTD = (1 << PD3);
                    break;
                case 3: // Jaune (Rouge + Vert)
                    PORTD = (1 << PD6) | (1 << PD5);
                    break;
                case 4: // Cyan (Vert + Bleu)
                    PORTD = (1 << PD5) | (1 << PD3);
                    break;
                case 5: // Magenta (Rouge + Bleu)
                    PORTD = (1 << PD6) | (1 << PD3);
                    break;
                case 6: // Blanc (toutes les couleurs)
                    PORTD = (1 << PD6) | (1 << PD5) | (1 << PD3);
                    break;
            }
            _delay_ms(100);  // Vitesse du disco (ajustez)
        }
        
        PORTD = 0;  // Éteint la LED RGB à la fin
        // Le programme reste bloqué ici (step++ dans main)
    }
    else // Clignotement rouge
    {
        for (char i = 0; i < 3; i++)
        {
            PORTD |= (1 << PD5);  // Rouge ON (PD5)
            _delay_ms(200);
            PORTD = 0;            // RGB OFF
            _delay_ms(200);
        }
        
        // Réactive le Timer0 pour relancer l'effet de chargement
        TIMSK0 |= (1 << OCIE0A);
    }
}
