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
        uint16_t adc = adc_read(8);
        /* DEBUG
        uart_printstr("ADC : ");
        uart_print_nbr(adc);
        uart_printstr(" | TEMP : "); */
        // Constantes ajustées pour cette puce spécifique
        // adc ≈ 332 devrait donner environ 23°C
        float temp = (adc - 308.31) / 1.03;
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
temp_celsius = (ADC - OFFSET) / GAIN

OFFSET (Tos) :
C'est la valeur ADC brute correspondant à 0°C
Chaque puce ATmega328P a un offset légèrement différent (non calibré en usine)

GAIN (k) :
C'est la sensibilité -> combien la valeur ADC change par degré Celsius
GAIN = 1.0 signifie : 1 LSB ≈ 1°C

1 LSB = 1 unité de la valeur ADC
C'est la plus petite variation mesurable par le convertisseu

p.256 |  Table 24-2 de la datasheet :
            -45°C → 242mV
            +25°C → 314mV
            +85°C → 380mV
Calcul avec référence interne 1.1V
    ADC@-45°C = (242 / 1100) × 1024 ≈ 225
    ADC@25°C = (314mV / 1100mV) × 1024 ≈ 292
    ADC@85°C = (380 / 1100) × 1024 ≈ 354
GAIN théorique :
    Entre 25°C et 85°C : GAIN = (354 - 292) / (85 - 25) = 62 / 60 ≈ 1.03 <====== YES
    Entre -45°C et 85°C : GAIN = (354 - 225) / (85 - (-45)) = 129 / 130 ≈ 0.99
    Entre -45°C et 25°C : GAIN = (292 - 225) / (25 - (-45)) = 67 / 70 ≈ 0.95
OFFSET théorique :
    OFFSET = 292 - 25 × 1.03 = 266.25


APPLICATION : T = (ADC@25 - OFFSET) / GAIN =  (292 - 266.25) / 1.03 = 25

MAIS : 
p.257 |  The temperature measurement is based on an on-chip temperature sensor... 
=> calibrer le capteur afin de compenser les variations du processus

REALITE :

ADC : 332 | TEMP : 63
ADC : 331 | TEMP : 62
ADC : 331 | TEMP : 62
ADC : 332 | TEMP : 63
ADC : 332 | TEMP : 63
ADC : 332 | TEMP : 63
ADC : 332 | TEMP : 63
ADC : 332 | TEMP : 63
ADC : 332 | TEMP : 63
ADC : 331 | TEMP : 62

DONC ADAPTATION : 
    23 = (332 - OFFSET) / 1.03
    OFFSET = 332 - (23 × 1.03) = 332 - 23.69 ≈ 308.31

RESULTAT :

ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 331 | TEMP : 22
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 332 | TEMP : 23
ADC : 331 | TEMP : 22

*/