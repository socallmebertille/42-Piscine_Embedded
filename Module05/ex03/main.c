#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    // p.257 |  Voltage Reference Selections for ADC
    // Référence interne fixe de 1.1V meilleur car mesure de petites tensions (0-1V)
    // avec ADC = (Vin / Vref) * 1024 = résolution de mesure et Vin = 0,25V = 25°C
    // si AVCC -> Vref = 5 -> ADC ≈ 51 
    // si interne -> Vref = 1.1 -> ADC ≈ 232
    ADMUX = (1 << REFS1) | (1 << REFS0); 

    ADCSRA = (1 << ADEN);               // ADC activé
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128

    // p. 258 |  Table 24-4. Input Channel Selections
    // pas de liaison avec une entrée analogique physique car on veut un capteur interne
    // soit ADC8 -> (1) For Temperature Sensor -> usage : température de la puce (CPU)
}

uint16_t adc_read(uint8_t peripheral)
{
    ADMUX = (ADMUX & 0xF0) | (peripheral & 0x0F); // MAJ nibble bas avec la bonne broche

    ADCSRA |= (1 << ADSC);              // lance la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin
    return ((uint16_t)ADCH << 8) | ADCL;// lecture de conversion sur 10 bits
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
        // ADC8 = capteur température interne
        uint16_t raw = adc_read(8);
        float temp = (raw - 324.31) / 1.22;
        uart_print_nbr((int)temp);
        uart_printstr("\r\n");

        _delay_ms(20);
    }
}
