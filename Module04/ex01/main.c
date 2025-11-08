#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PWM_TOP 999

/*

        Timer1 : F_PWM​ = 2000 Hz = 2000 interruptions
---> pour pouvoir faire 200 changements de duty cycle (0% -> 100% -> 0% en 1sec)
en sachant que l'oeil humain remarque un clignottement a envirion 100 Hz

PWM_TOP = (F_CPU / (prescaler * F_PWM)) - 1 = (16 000 000 / (8 * 2000)) - 1 = 999

        Timer0 : F_interrupt = 2000 Hz

OCR0A = 16000000/(64*2000) - 1 = 125 - 1 = 124

ISR appelée 2000 fois/sec ===> duty varie de 0 à 999 (1000 steps)

Montée : 1000 steps / 2000 Hz = 0.5 sec     +     Descente : 1000 steps / 2000 Hz = 0.5 sec
                            = Cycle complet : 1 seconde

*/

void duty_cycle_init(void)           // Timer1
{
    DDRB |= (1 << PB1);                       // LED D2 configuration
    PORTB &= ~(1 << PB1);                     // Éteint PB1

    TCCR1A |= (1 << COM1A1);                  // Clear on Compare Match
    TCCR1A |= (1 << WGM11);                   // Fast PWM Mode 14
    TCCR1B |= (1 << WGM13) | (1 << WGM12); 
    TCCR1B |= (1 << CS11);                    // Prescaler /8
    ICR1 = PWM_TOP;                           // TOP -> 999
    OCR1A = 0;                                // Compare Match départ à 0%
}

void periodic_interrupt_init(void)   // Timer0
{
    TCCR0A |= (1 << WGM01);                   // CTC Mode 2
    TCCR0B = (1 << CS01) | (1 << CS00);       // prescaler /64
    OCR0A = 124;                              // ≈ 2000 Hz -> interruption toutes les 0,5 ms
    TIMSK0 = (1 << OCIE0A);                   // activer interruption compare match A
}

static volatile uint16_t duty = 0;
static volatile int8_t direction = 1;

ISR(TIMER0_COMPA_vect)
{
    duty += direction;
    if (duty >= PWM_TOP)
    {
        duty = PWM_TOP;
        direction = -1;
    }
    else if (duty <= 0)
    {
        duty = 0;
        direction = 1;
    }
    OCR1A = duty;
}

int main(void)
{
    cli();              // Ignorer les interruptions

    duty_cycle_init();
    periodic_interrupt_init();

    sei();              // Autoriser les interruptions

    while (1) {}
}
