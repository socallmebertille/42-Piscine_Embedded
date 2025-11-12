#include "main.h"

void adc_init(void)
{
    PRR &= ~(1 << PRADC);               // active le module ADC

    // p.257 |  Voltage Reference Selections for ADC
    // Référence interne fixe de 1.1V meilleur car mesure de petites tensions (0-1V)
    // avec ADC = (Vin / Vref) * 1024 = résolution de mesure et Vin = 0,25V = 25°C
    // si AVCC -> Vref = 5 -> ADC ≈ 51
    // si interne -> Vref = 1.1 -> ADC ≈ 232
    // ADLAR = 0 pour alignement à droite (lecture 10 bits)
    ADMUX = (1 << REFS1) | (1 << REFS0);
    ADMUX &= ~(1 << ADLAR); 

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
        // ADC8 = capteur température interne
        uint16_t raw = adc_read(8);
        // Constantes ajustées pour cette puce spécifique
        // raw ≈ 332 devrait donner environ 23°C
        float temp = (raw - 309.0) / 1.0;
        uart_print_nbr((int)temp);
        uart_printstr("\r\n");

        _delay_ms(20);
    }
}

/*

p.257 |  24.8 Temperature Measurement

FORMULE de conversion température en celcius est :
T = { [(ADCH << 8) | ADCL] - TOS } / k
    <=>
temp_celsius = (adc_raw - OFFSET) / GAIN

OFFSET (Tos = 309.0) :
C'est la valeur ADC brute correspondant à 0°C
Chaque puce ATmega328P a un offset légèrement différent (non calibré en usine)

GAIN (k = 1.0) :
C'est la sensibilité -> combien la valeur ADC change par degré Celsius
GAIN = 1.0 signifie : 1 LSB ≈ 1°C

1 LSB = 1 unité de la valeur ADC
C'est la plus petite variation mesurable par le convertisseu

*/