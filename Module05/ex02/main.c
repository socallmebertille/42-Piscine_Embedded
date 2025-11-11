#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    ADMUX = (1 << REFS0);               // AVCC + alignement à droite

    ADCSRA = (1 << ADEN);               // ADC activé
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128

    DIDR0 |= (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D); // désactive les entrées digitales des broches
}

uint16_t adc_read(uint8_t peripheral)
{
    ADMUX = (ADMUX & 0xF0) | (peripheral & 0x0F); // MAJ nibble bas avec la bonne broche

    ADCSRA |= (1 << ADSC);              // lance la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin
    
    // Lire d'abord ADCL, puis ADCH (10 bits) pour ADLR = 0
    // ADCH : [ x  x  x  x  x  x  A9 A8 ]
    // ADCL : [ A7 A6 A5 A4 A3 A2 A1 A0 ]
    uint16_t result = ADCL;
    result |= ((uint16_t)ADCH << 8);

    return result;
}

void uart_print_nbr(uint16_t val)
{
    if (val >= 10)
        uart_print_nbr(val / 10);
    uart_tx('0' + (val % 10));
}

int main(void)
{
    uart_init();
    adc_init();

    while (1)
    {
        // ADC0 : mettre MUX1=0 et MUX0=0
        uint16_t value = adc_read(0);   // 0 = 0b0000
        uart_print_nbr(value);
        uart_printstr(", ");

        // ADC1 : mettre MUX1=0 et MUX0=1
        value = adc_read(1);           // 1 = 0b0001
        uart_print_nbr(value);
        uart_printstr(", ");

        // ADC2 : mettre MUX1=1 et MUX0=0
        value = adc_read(2);           // 2 = 0b0010
        uart_print_nbr(value);
        uart_printstr("\r\n");

        _delay_ms(20);
    }
}
