#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    ADMUX = (1 << REFS0) | (1 << ADLAR); // AVCC as reference + ADLAR bit à gauche

    ADCSRA = (1 << ADEN);               // ADC activé
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128

    DIDR0 |= (1 << ADC0D);              // ADC0 pin enable
}

uint8_t adc_read(uint8_t peripheral)
{
    ADMUX = (ADMUX & 0xF0) | (peripheral & 0x0F); // MAJ nibble bas avec la bonne broche

    ADCSRA |= (1 << ADSC);              // lance la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin
    return ADCH;                        // lecture de conversion sur 8 bits
}
