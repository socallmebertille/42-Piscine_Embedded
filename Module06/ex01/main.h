#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/twi.h>
# include <util/delay.h>

// ======== UART =========

# define F_CPU          16000000UL
# define BAUD           115200
# define UART_BAUDRATE  (F_CPU / (16UL * BAUD))

void    uart_init(void);
void    uart_tx(char c);
char    uart_rx(void);
void    uart_print_hex(uint8_t val);
void    uart_print_nbr(uint32_t val);
void    uart_printstr(const char* str);

// ======== I2C =========

# define AHT20_ADDR     0x38
# define SCL_FREQ       100000UL

# define STATUS         (TWSR & 0xF8) // registre status sans bits de prescaler

void    i2c_init(uint16_t kHz);
void    i2c_start(void);
void    i2c_stop(void);
void    i2c_write_status(char *action);

void    i2c_write(unsigned char data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void    i2c_read(void);
void    print_hex_value(char c);

#endif