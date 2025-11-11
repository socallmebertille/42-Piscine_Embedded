#include "main.h"

void adc_init(void) // convertisseur analogique–numérique
{
    // p.55  |  PRR Power Reduction Register
    PRR &= ~(1 << PRADC);               // active le module ADC

    // p.257 |  ADC Multiplexer Selection Register
    ADMUX = (1 << REFS0);               // AVCC as a reference (AVCC = tension max)
    // aucun bit de MUX[3:0] (analog channel selection) à mettre à 1 
    // car on veut la broche ADC0 reliée au potentiomètre RV1
    ADMUX |= (1 << ADLAR);              // lire les bits ajustés à gauche

    // p.258 |  ADC Control and Status Register A
    ADCSRA = (1 << ADEN);               // ADC activé
    // p.249 |  requires an input clock frequency between 50kHz and 200kHz
    // f_ADC ​= 16 000 kHz / 128 = 125 kHz ∈ [50;200] kHz
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) 
            | (1 << ADPS0);              // prescaler /128

    // p.261 |  Digital Input Disable Register 0
    DIDR0 |= (1 << ADC0D);               // désactive l'entrée digitale sur ADC0 pour consommer moins d'énergie
}

uint8_t adc_read(void)
{
    // p.258 |  ADC Control and Status Register A
    ADCSRA |= (1 << ADSC);              // Start : lancer la conversion
    while (ADCSRA & (1 << ADSC));       // attendre la fin
    return ADCH;                        // lire les 8 bits (alignés à gauche)
    // return ADCL;                     // si flag ADLAR à 0 -> bits alignés à droite
}

void uart_print_hex(uint8_t val)
{
    const char hex[] = "0123456789abcdef";
    uart_tx(hex[val >> 4]);             // nibble haut -> 4 bits haut
    uart_tx(hex[val & 0x0F]);           // nibble bas -> 4 bits bas (0x0F = 0b1111 = 0000 1111)
    uart_tx('\r');
    uart_tx('\n');
}

int main(void)
{
    uart_init();
    adc_init();

    while (1)
    {
        uint8_t value = adc_read();
        uart_print_hex(value);
        _delay_ms(20);
    }
}
