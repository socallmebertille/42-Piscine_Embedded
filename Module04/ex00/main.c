#include <avr/io.h>
#include <avr/interrupt.h>

void interrupt_init(void)
{
    // LED D1 configuration
    DDRB |= (1 << PB0);     // PB0 en sortie
    PORTB &= ~(1 << PB0);   // LED éteinte
    
    // SW1 configuration
    DDRD &= ~(1 << PD2);    // PD2 en entrée
    PORTD |= (1 << PD2);    // Active pull-up

    // External Interrupt Control Register A
    EICRA |= (1 << ISC01);  // The falling edge of INT0 generates an interrupt request.
    // External Interrupt Mask Register
    EIMSK |= (1 << INT0);   // Enable INT0 link to PD2 (=SW1)
}

ISR(INT0_vect)
{
    // Code à exécuter quand l'interrupt se déclenche
    PORTB ^= (1 << PB0);
}

int main(void)
{
    cli(); // Ignorer les interruptions
    interrupt_init();
    sei(); // Autoriser les interruptions

    while (1) {}
}
