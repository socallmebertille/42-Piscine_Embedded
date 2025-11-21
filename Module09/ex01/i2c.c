#include "main.h"

void i2c_init(uint16_t kHz)         // init => Inter-Integrated Circuit | Two-Wire Interface
{
    TWSR = 0; // 0 = prescaler /1
    uint32_t scl = (uint32_t)kHz * 1000UL; // SCL = CPU_Clock / (16 + 2*TWBR*Prescaler)
    if (scl == 0) scl = 100000UL;
    uint32_t twbr_calc = 0;
    if (F_CPU > scl * 16UL)
        twbr_calc = (F_CPU / scl - 16UL) / 2UL;
    if (twbr_calc > 255UL) twbr_calc = 255UL;
    TWBR = (uint8_t)twbr_calc;
    TWCR = (1 << TWEN); // TWEN = active TWI
}

void i2c_start(void)                // step 1:3.1 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // clear TWINT flag | génère START | active TWI
    while (!(TWCR & (1 << TWINT))); // attendre que START soit effectivement transmis                                // step 2 p.225
    
    if (STATUS != TW_START && STATUS != TW_REP_START) // vérifier la valeur des status retourner
    {
        i2c_write_status("ERROR");
        return;
    }
    else
        i2c_write_status("START");
}

void i2c_stop(void)                  // step 7.2 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // TWSTO = génère STOP
}

void i2c_write_status(char *action)
{
    if (action) { uart_printstr(action); uart_printstr(" "); }
    uart_printstr("status: 0x");
    uart_print_hex(STATUS);
    uart_printstr("\r\n");
}

void i2c_write(unsigned char data)  // step 3.2:5.1 p.225
{
    TWDR = data; // mettre le byte dans TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);  // lancer la transmission = remise des flags = clear + active TWI
    while (!(TWCR & (1 << TWINT))); // attendre la fin de la transmission

    // p.227:30 |  status possibles attendus : 
    //          TW_MT_SLA_ACK (0x18) = SLA+W transmis + slave a répondu ACK
    //          TW_MT_DATA_ACK (0x28) = Data byte transmis + slave a répondu ACK
    //          TW_MR_SLA_ACK (0x40) = SLA+R transmis + slaver a répondu ACK
    if (STATUS == TW_MT_SLA_ACK)
        i2c_write_status("SLA+W");
    else if (STATUS == TW_MT_DATA_ACK)
        i2c_write_status("WRITE");
    else if (STATUS == TW_MR_SLA_ACK)
        i2c_write_status("SLA+R");
    else
    {
        i2c_write_status("ERROR");
        return;
    }
}

uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // TWEA = ACK
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN); // pas de TWEA = NACK
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}
