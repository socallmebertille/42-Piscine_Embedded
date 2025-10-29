#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB0);
    PORTB |= (1 << PB0);
    return 0;
}
