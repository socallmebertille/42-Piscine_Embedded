#include "main.h"

int main(void)
{
    uart_init();
    i2c_init();

    uart_printstr("\r\n=== I2C Test ===\r\n");

    i2c_start();
    i2c_stop();

    while (1) {}
}
