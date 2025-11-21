#include "main.h"

/*
    schema microcontroller : I2C 16-Bit I/O Expander modele PCA9555PW

    https://www.alldatasheet.com/datasheet-pdf/download/86227/PHILIPS/PCA9555PW.html
*/

static void pca95555pw_write(uint8_t IO_register, uint8_t data)
{
    i2c_start();

    i2c_write(PCA9555PW_ADDR << 1); // Device Address + mode WRITE
    i2c_write(IO_register);         // Register selection
    i2c_write(data);                // Data

    i2c_stop();

}

static uint8_t pca95555pw_read(uint8_t IO_register)
{
    uint8_t data;

    i2c_start();
    i2c_write(PCA9555PW_ADDR << 1);       // Device Address + mode WRITE
    i2c_write(IO_register);               // Register selection
    
    i2c_start();
    i2c_write((PCA9555PW_ADDR << 1) | 1); // Device Address + mode READ
    data = i2c_read_nack();               // NACK car dernier octet

    i2c_stop();

    return data;
}

static void display_binary(uint8_t val)
{
    uint8_t output = 0b11111111; // Tout éteint (active-low)
    
    if (val & (1 << 0)) output &= ~(1 << 3); // D9  = bit 3 = 0
    if (val & (1 << 1)) output &= ~(1 << 2); // D10 = bit 2 = 0
    if (val & (1 << 2)) output &= ~(1 << 1); // D11 = bit 1 = 0
    
    pca95555pw_write(0x02, output); // 0x02 = Output Port 0 (écrire sur les pins)
}

int main(void)
{
    uint8_t val = 0;
    
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    // 0x06  = Configuration Port 0 (0=output, 1=input)
    // IO0_0 = SW3 (input,  bit = 1)
    // IO0_1 = D11 (output, bit = 0)
    // IO0_2 = D10 (output, bit = 0)
    // IO0_3 = D9  (output, bit = 0)
    pca95555pw_write(0x06, 0b11110001);

    while (1)
    {
        // 0x00 = Input Port 0 (lire l'état des pins)
        if (!(pca95555pw_read(0x00) & (1 << 0)))        // SW3 pressé (active-low)
        {
            val = (val + 1) % 8;
            display_binary(val);
            
            while (!(pca95555pw_read(0x00) & (1 << 0))) // Attendre relâchement
                _delay_ms(10);
            _delay_ms(50);                              // Debounce effect
        }
    }
}
