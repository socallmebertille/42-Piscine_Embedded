#include "main.h"

void i2c_init(uint32_t scl_freq_hz) // init => Inter-Integrated Circuit | Two-Wire Interface
{
    uint32_t twbr = (F_CPU / scl_freq_hz - 16) / 2; // SCL = CPU_Clock / (16 + 2*TWBR*Prescaler)
    TWBR = (uint8_t)twbr;
    TWSR = 0; // 0 = prescaler /1
    TWCR = (1 << TWEN); // TWEN = active TWI
}

void i2c_start(void)                // step 1:3.1 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // clear TWINT flag | génère START | active TWI
    while (!(TWCR & (1 << TWINT))); // attendre que START soit effectivement transmis                                // step 2 p.225
    
    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START) // vérifier la valeur des status retourner
    {
        uart_printstr("ERROR: START failed\r\n");
        return;
    }
}

void i2c_stop(void)                  // step 7.2 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // TWSTO = génère STOP
    while (TWCR & (1 << TWSTO)); // attendre que STOP soit effectivement transmis
}
