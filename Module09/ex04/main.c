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
    _delay_ms(5);
}

int main(void)
{
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    // IO0_7 = DP4 = CA4 & IO0_6 = DP3 = CA3 (output, bit = 0)
    pca95555pw_write(CONFIG_REG_0, 0b00000000);
    // IO1_0:7 = 7 segments + DPX  (output, bit = 0)
    pca95555pw_write(CONFIG_REG_1, 0b00000000);

    while (1)
    {
        display_digit(0b10111111, FOUR); // IO0_6 = DP3 = CA3 = "4" (output, bit = 1)
        display_digit(0b01111111, TWO);  // IO0_7 = DP4 = CA4 = "2" (output, bit = 1)
    }
}
