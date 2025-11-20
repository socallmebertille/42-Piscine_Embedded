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

// ======== SPI ==========

void            SPI_MasterInit(void);
void            SPI_MasterTransmit(char cData);
void            send_action_frame(uint8_t action);
void            led_send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);

#endif