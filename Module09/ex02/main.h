#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>

// ======== UART =========

# define F_CPU         16000000UL
# define BAUD          115200
# define UART_BAUDRATE (F_CPU / (16UL * BAUD))

void			uart_init(void);
void			uart_tx(char c);
char			uart_rx(void);
void			uart_print_hex(uint8_t val);
void			uart_print_nbr(uint32_t val);
void			uart_printstr(const char *str);
void			uart_nibble(uint8_t n);
void			uart_addr(unsigned int addr);

// ======== I2C =========

# define PCA9555PW_ADDR 0x20 // Device Address

# define CONFIG_REG_0   0x06
# define INPUT_PORT_0   0x00
# define OUTPUT_PORT_0  0x02

# define CONFIG_REG_1   0x07
# define OUTPUT_PORT_1  0x03

/* macros de code status present dans twi.h */
# define TW_START       0x08
# define TW_REP_START   0x10
# define TW_MT_SLA_ACK  0x18
# define TW_MT_DATA_ACK 0x28
# define TW_MR_SLA_ACK  0x40

# define SCL_FREQ       100000UL

# define STATUS         (TWSR & 0xF8) // registre status sans bits de prescaler

void    i2c_init(uint16_t kHz);
void    i2c_start(void);
void    i2c_stop(void);
void    i2c_write_status(char *action);

void    i2c_write(unsigned char data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

#endif