#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void interrupt_init(void)
{
    // LEDs D1:4 configuration
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);        // PB0:4 en sortie
    PORTB = 0;                                                        // LEDs éteintes
    
    // SW1 et SW2 configuration
    DDRD &= ~((1 << PD2) | (1 << PD4));    // PD2 et PD4 en entrée
    PORTD |= (1 << PD2) | (1 << PD4);    // Active pull-up

    // SW1 Interrupt
    // External Interrupt Control Register A
    EICRA |= (1 << ISC01);  // The falling edge of INT0 generates an interrupt request.
    // External Interrupt Mask Register
    EIMSK |= (1 << INT0);   // Enable INT0 link to PD2 (=SW1)

    // SW2 Interrupt
    // PD4 Alternate Function :
    // XCK (USART External Clock Input/Output)
    // T0 (Timer/Counter 0 External Counter Input)
    // PCINT20 (Pin Change Interrupt 20)

    // Pin Change Interrupt Control Register
    PCICR |= (1 << PCIE2);    // Enable les PCINT du port D
    // Pin Change Mask Register 2
    PCMSK2 |= (1 << PCINT20); // Pin change interrupt on the corresponding Pin (=PD4)
}

static int8_t value = 0;

void set_leds(void)
{
    PORTB = 0;
    if (value & (1 << 0)) PORTB |= (1 << PB0);
    if (value & (1 << 1)) PORTB |= (1 << PB1);
    if (value & (1 << 2)) PORTB |= (1 << PB2);
    if (value & (1 << 3)) PORTB |= (1 << PB4);
}

ISR(INT0_vect) // Code à exécuter quand l'interrupt lie a SW1 se déclenche
{
    _delay_ms(20); // attendre la fin des rebonds
    if (!(PIND & (1 << PD2))) { // si toujours appuyé
        value++;
        if (value > 15) value = 0;
        set_leds();
    }
}

ISR(PCINT2_vect) // Code à exécuter quand l'interrupt lie a SW2 se déclenche
{
    _delay_ms(20); // attendre la fin des rebonds
    if (!(PIND & (1 << PD4))) { // si toujours appuyé
        value--;
        if (value < 0) value = 15;
        set_leds();
    }
}

int main(void)
{
    cli(); // Ignorer les interruptions
    interrupt_init();
    sei(); // Autoriser les interruptions

    while (1) {}
}