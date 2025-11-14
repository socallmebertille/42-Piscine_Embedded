#include "main.h"

// https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf?utm_source=chatgpt.com

void aht20_trigger_measure(void) //p.8
{
    i2c_write((AHT20_ADDR << 1) | 0);   // SLA+W = 0111 0000

    i2c_write(0xAC); // 1010 1100
    i2c_write(0x33); // 0011 0011
    i2c_write(0x00); // 0000 0000
}

void aht20_read_temp_hum_data(void)
{
    i2c_write((AHT20_ADDR << 1) | 1);   // SLA+R = 0111 0001

    i2c_read();         // lit les 7 octets bruts
}

int main(void)
{
    uart_init();
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    _delay_ms(100);

    while (1)
    {
        i2c_start();
        aht20_trigger_measure();
        i2c_stop();

        _delay_ms(80);      // p.3 | step 3 -> temps de mesure AHT20

        i2c_start();
        aht20_read_temp_hum_data();
        i2c_stop();

        _delay_ms(2000);     // p.5 |  it is recommended to measure data every 2 seconds
    }
}
