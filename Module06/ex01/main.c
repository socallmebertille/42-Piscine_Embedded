#include "main.h"

int main(void)
{
    uart_init();
    i2c_init(100);   // 100 kHz
    _delay_ms(100);

    while (1)
    {
        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 0);   // SLA+W

        i2c_write(0xAC);   // trigger measurement
        i2c_write(0x33);
        i2c_write(0x00);

        i2c_stop();

        _delay_ms(80);      // temps de mesure AHT20

        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 1);   // SLA+R

        i2c_read();         // lit les 7 octets bruts

        i2c_stop();

        _delay_ms(500);     // recommence toutes les 0.5 s
    }
}
