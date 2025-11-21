#include "main.h"

// https://www.ti.com/lit/ds/symlink/pca9555.pdf?ts=1763715157311&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252Fde-de%252FPCA9555%252Fpart-details%252FPCA9555PWR

static void pca95555pw_write(uint8_t output_registre, uint8_t data)
{
    i2c_start();

    i2c_write(PCA9555PW_ADDR << 1); // WRITE
    i2c_write(output_registre);
    i2c_write(data);

    i2c_stop();

}

int main(void)
{
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    // CONFIGURE PORT 1 (0x07 = Configuration register port 1)
    // Bit = 0 → OUTPUT
    // On veut IO0_3 = output → bit2 = 0
    // Donc masque : 11111011 = 0xFB       | 11110111 = 0xF7
    pca95555pw_write(0x06, 0b11110111);

    while (1)
    {
        pca95555pw_write(0x02, 0b11110111); // LED D9 ON -> IO0_3 = 0 (active low)

        _delay_ms(500);

        pca95555pw_write(0x02, 0b11111111); // LED D9 OFF -> IO0_3 = 1

        _delay_ms(500);
    }
}
