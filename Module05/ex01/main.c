#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    ADMUX = (1 << REFS0) | (1 << ADLAR); // AVCC + alignement à gauche

    ADCSRA = (1 << ADEN);               // ADC activé
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128

    DIDR0 |= (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D); // désactive les entrées digitales des broches
}

uint8_t adc_read(void)
{
    ADCSRA |= (1 << ADSC);              // lance la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin
    return ADCH;                        // lire les 8 bits
}

void uart_print_hex(uint8_t val)
{
    const char hex[] = "0123456789abcdef";
    uart_tx(hex[val >> 4]);             // nibble haut -> 4 bits haut
    uart_tx(hex[val & 0x0F]);           // nibble bas -> 4 bits bas (0x0F = 0b1111 = 0000 1111)
}

int main(void)
{
    uart_init();
    adc_init();

    while (1)
    {
        // ADC0 : mettre MUX1=0 et MUX0=0
        ADMUX &= ~(1 << MUX1);                  // efface le bit MUX1
        uint8_t value = adc_read();
        uart_print_hex(value);
        uart_printstr(", ");

        // ADC1 : mettre MUX1=0 et MUX0=1
        ADMUX |= (1 << MUX0);                   // met MUX0 à 1
        value = adc_read();
        uart_print_hex(value);
        uart_printstr(", ");

        // ADC2 : mettre MUX1=1 et MUX0=0
        ADMUX &= ~(1 << MUX0);                  // efface le bit MUX0
        ADMUX |= (1 << MUX1);                   // met MUX1 à 1
        value = adc_read();
        uart_print_hex(value);
        uart_printstr("\r\n");

        _delay_ms(20);
    }
}
