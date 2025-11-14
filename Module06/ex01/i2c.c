#include "main.h"

void i2c_init(uint16_t kHz)         // init => Inter-Integrated Circuit | Two-Wire Interface
{
    uint32_t scl = (uint32_t)kHz * 1000UL; // SCL = CPU_Clock / (16 + 2*TWBR*Prescaler)
    uint32_t twbr = (F_CPU / scl - 16) / 2;
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

void i2c_write_status(char *action)
{
    if (action)
    {
        uart_printstr(action);
        uart_printstr(" ");
    }
    uart_printstr("status: 0x");
    uart_print_hex(TWSR & 0xF8);
    uart_printstr("\r\n");
}

void i2c_write(unsigned char data)  // step 3.2:5.1 p.225
{
    TWDR = data; // mettre le byte dans TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);  // lancer la transmission = remise des flags = clear + active TWI
    while (!(TWCR & (1 << TWINT))); // attendre la fin de la transmission
    // if ((TWSR & 0xF8) != TW_MT_SLA_ACK) // vérifier que le slave a répondu ACK
    // {
    //     uart_printstr("ERROR: Slave NACK\r\n");
    //     return;
    // }
    // i2c_write_status("WRITE");
}

void i2c_read(void)
{
    unsigned char data;

    for (uint8_t i = 0; i < 7; i++)
    {
        if (i < 6)
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // ACK
        else
            TWCR = (1 << TWINT) | (1 << TWEN);               // NACK

        while (!(TWCR & (1 << TWINT))); // attendre

        data = TWDR;

        print_hex_value(data);          // afficher 1 octet
        if (i < 6)
            uart_printstr(" ");         // espace entre les bytes
    }
    uart_printstr("\r\n");
}

void print_hex_value(char c)
{
    uart_print_hex((uint8_t)c);
}
