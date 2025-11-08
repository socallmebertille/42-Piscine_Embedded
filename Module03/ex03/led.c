#include "main.h"

void led_init(void)
{
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3); // LEDs RGB en sortie

    // Initialisation Timer0 car broche PD5 et PD6 reliées a OC0A:B
    TCCR0A |= (1 << WGM01) | (1 << WGM00);   // Mode Fast PWM
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // PWM sur OC0A (PD6) et OC0B (PD5)

    TCCR0B |= (1 << CS01) | (1 << CS00);     // Prescaler /64

    // Initialisation Timer2 car broche PD3 reliee a OC2B
    TCCR2A |= (1 << WGM21) | (1 << WGM20);   // Mode Fast PWM
    TCCR2A |= (1 << COM2B1);                 // PWM sur OC2B (PD3)

    TCCR2B |= (1 << CS22);                   // Prescaler /64
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; // Rouge (PD5)
    OCR0A = g; // Vert (PD6)
    OCR2B = b; // Bleu (PD3)
}

void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

static uint8_t get_color_in_255(char *color)
{
    uint8_t result = 0;
    
    // Premier caractère hex
    if (color[0] >= '0' && color[0] <= '9')
        result = (color[0] - '0') * 16;
    else if (color[0] >= 'A' && color[0] <= 'F')
        result = (color[0] - 'A' + 10) * 16;
    
    // Deuxième caractère hex
    if (color[1] >= '0' && color[1] <= '9')
        result += (color[1] - '0');
    else if (color[1] >= 'A' && color[1] <= 'F')
        result += (color[1] - 'A' + 10);
    
    return result;
}

void set_led(char *color)
{
    // Sauter le '#'
    color++;
    
    // Extraire RR, GG, BB
    uint8_t r = get_color_in_255(color);      // RR
    uint8_t g = get_color_in_255(color + 2);  // GG
    uint8_t b = get_color_in_255(color + 4);  // BB
    
    set_rgb(r, g, b);
}
