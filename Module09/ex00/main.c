#include "main.h"

/*
    schema microcontroller : I2C 16-Bit I/O Expander modele PCA9555PW

    https://www.alldatasheet.com/datasheet-pdf/download/86227/PHILIPS/PCA9555PW.html
*/

static void pca95555pw_write(uint8_t IO_register, uint8_t data)
{
    i2c_start();

    // p.7 |  Device address -> [0|1|0|0|A0|A1|A2|R/W] <=> addr = 0100 000 = 0x20
    // où A0:2 = configuration physique des trois interrupteurs/sitchs SW5 donc les mettre a 0 manuellement ou risque de pas pouvoir parler a la led

    i2c_write(PCA9555PW_ADDR << 1); // Device Address + mode WRITE
    i2c_write(IO_register);         // Register selection
    i2c_write(data);                // Data

    i2c_stop();

}

int main(void)
{
    i2c_init(100);   // 100 kHz => vitesse standard I2C

    // OPTIONNEL : voir en temps reel les status code
    uart_init();

    // p.3 |  PIN Configuration -> D9 -> IO0_3 -> PIN number 4
    // p.6 |  Registers 2 and 3 — Output Port Registers -> [O0_7|O0_6|O0_5|O0_4|O0_3|O0_2|O0_1|O0_0] = 1111 1111
    // => On veut IO0_3 = output → bit 3 = 0 ou 1 = allume ou eteint

    // p.8  | Figure 7. WRITE to configuration registers => config = 0000 0110 = 0x06 & data = 1111 0111 = 0xF7
    pca95555pw_write(0x06, 0b11110111);

    while (1)
    {
        // p.8  | Figure 6. WRITE to output port registers => Output register = 0000 0010 = 0x02 & data = bit 3 = 0 ou 1
        pca95555pw_write(0x02, 0b11110111); // LED D9 ON -> IO0_3 = 0 (active low)

        _delay_ms(500);

        pca95555pw_write(0x02, 0b11111111); // LED D9 OFF -> IO0_3 = 1

        _delay_ms(500);
    }
}
