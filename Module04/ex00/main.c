#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "isr.h"

static volatile uint8_t pressed = 0;

void interrupt_init(void)
{
    // LED D1 configuration
    DDRB |= (1 << PB0);     // PB0 en sortie
    PORTB &= ~(1 << PB0);   // LED éteinte
    
    // SW1 configuration
    DDRD &= ~(1 << PD2);    // PD2 en entrée
    PORTD |= (1 << PD2);    // Active pull-up

    // External Interrupt Control Register A
    EICRA |= (1 << ISC01) | (1 << ISC00);  // The rising edge of INT0 generates an interrupt request.
    // External Interrupt Mask Register
    EIMSK |= (1 << INT0);                  // Enable INT0 link to PD2 (=SW1)
}

ISR_EXTERNAL_0
{
    // Code à exécuter quand l'interrupt se déclenche
    pressed = 1; // Signal à la boucle principale
}

int main(void)
{
    cli(); // Ignorer les interruptions
    interrupt_init();
    sei(); // Autoriser les interruptions

    while (1)
    {
        if (pressed)
        {
            _delay_ms(50);       // Anti-rebond
            PORTB ^= (1 << PB0); // Inverse l'état de la LED
            pressed = 0;         // Réinitialise le flag
        }
    }
}
