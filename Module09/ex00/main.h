#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>

// ======== I2C =========

/*
    schema microcontroller : I2C 16-Bit I/O Expander
    -> Address : [0|1|0|0|A0|A1|A2|RW] <=> addr = 0100 000 = 0x20
*/

# define PCA9555PW_ADDR 0x20

/*
    macros de code status present dans twi.h
*/

# define TW_START       0x08
# define TW_REP_START   0x10
# define TW_MT_SLA_ACK  0x18
# define TW_MT_DATA_ACK 0x28
# define TW_MR_SLA_ACK  0x40

# define SCL_FREQ       100000UL

# define STATUS         (TWSR & 0xF8) // registre status sans bits de prescaler

# define NB_MEAS        3

void    i2c_init(uint16_t kHz);
void    i2c_start(void);
void    i2c_stop(void);
void    i2c_write_status(char *action);

void    i2c_write(unsigned char data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

#endif