#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>

// ======== UART =========

# define BAUD           115200
# define UART_BAUDRATE  (F_CPU / (16 * BAUD))

void    uart_init(void);
void    uart_tx(char c);
char    uart_rx(void);
void    uart_printstr(const char* str);


#endif