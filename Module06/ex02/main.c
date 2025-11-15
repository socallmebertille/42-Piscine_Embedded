#include "main.h"

// https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf?utm_source=chatgpt.com

void aht20_calibration(void) // p.8 | Step 1
{
    i2c_start();
    i2c_write((AHT20_ADDR << 1) | 1);   // SLA+R = 0111 0001 = 0x71
    uint8_t status = i2c_read_nack();
    i2c_stop();

    // check calibration (bit 3)
    if (!(status & (1 << 3)))  // If bit 3 is NOT 1 (not calibrated)
    {
        // envoyer commande de calibration
        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 0);   // SLA+W = 0111 0000 = 0x70
        i2c_write(0xBE);
        i2c_write(0x08);
        i2c_write(0x00);
        i2c_stop();
        _delay_ms(10);
    }

}

void aht20_trigger_measure(void) // p.8 | Step 2
{
    i2c_start();

    i2c_write((AHT20_ADDR << 1) | 0);   // SLA+W = 0111 0000 = 0x70

    i2c_write(0xAC); // 1010 1100
    i2c_write(0x33); // 0011 0011
    i2c_write(0x00); // 0000 0000

    i2c_stop();
}

void aht20_read_temp_hum_data(void) // p.8
{
    i2c_start();

    i2c_write((AHT20_ADDR << 1) | 1);   // SLA+R = 0111 0001 = 0x71

    i2c_read();         // lit les 7 octets bruts

    i2c_stop();
}

int main(void)
{
    uart_init();
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    _delay_ms(40);   // p.8 | Step 1 :  Wait 40ms after power-on

    aht20_calibration();     // p.8 | Step 1

    while (1)
    {
        aht20_trigger_measure(); // p.8 | Step 2

        _delay_ms(80);      // p.8 | Step 3 -> temps de mesure AHT20

        aht20_read_temp_hum_data(); // p.8 | Step 4

        _delay_ms(2000);     // p.5 |  it is recommended to measure data every 2 seconds
    }
}
