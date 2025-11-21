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

static void display_digit(uint8_t position, uint8_t segments)
{
    pca95555pw_write(OUTPUT_PORT_1, 0b00000000);   // Éteindre segments
    pca95555pw_write(OUTPUT_PORT_0, position);     // Activer digit
    pca95555pw_write(OUTPUT_PORT_1, segments);     // Afficher segments
    _delay_ms(1);
}

static uint8_t get_digit(uint16_t nb, uint8_t idx)
{
    while (idx--)
        nb /= 10;
    return (nb % 10);
}

int main(void)
{
    uint8_t tab[10] = { ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE };
    uint8_t digit_mask[4] = {
        0b11101111,   // digit 1 (leftmost)
        0b11011111,   // digit 2
        0b10111111,   // digit 3
        0b01111111    // digit 4 (rightmost)
    };

    adc_init();
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    // IO0_7 = DP4 = CA4 & IO0_6 = DP3 = CA3 (output, bit = 0)
    pca95555pw_write(CONFIG_REG_0, 0b00000000);
    // IO1_0:7 = 7 segments + DPX  (output, bit = 0)
    pca95555pw_write(CONFIG_REG_1, 0b00000000);

    while (1)
    {
        uint16_t adc = adc_read(0);

        for (uint8_t i = 0; i < 4; i++)
            display_digit(digit_mask[i], tab[get_digit(adc, 3 - i)]);
    }
}
