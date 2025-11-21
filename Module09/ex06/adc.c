#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    ADMUX = (1 << REFS0);               // AVCC as reference

    ADCSRA = (1 << ADEN);               // ADC activé
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128

    DIDR0 |= (1 << ADC0D);              // ADC0 pin enable
}

uint16_t adc_read(uint8_t peripheral)
{
    ADMUX = (ADMUX & 0xF0) | (peripheral & 0x0F); // MAJ nibble bas avec la bonne broche

    ADCSRA |= (1 << ADSC);              // lance la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin

    uint16_t result = ADCL;
    result |= ((uint16_t)ADCH << 8);

    return result;
}
