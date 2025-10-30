#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB0); // configure PB0 as an output by setting its first bit to 1 because it goes in the direction of the ground (=GND)
    PORTB |= (1 << PB0); // sets PB0 to HIGH (5V) → turns on the LED if it is connected to GND
    return 0;
}
