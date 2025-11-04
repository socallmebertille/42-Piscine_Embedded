#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>
# include <avr/interrupt.h>

// ======== UART =========

# define BAUD           115200
# define UART_BAUDRATE  (F_CPU / (16 * BAUD))

void    uart_init(void);
void    uart_tx(char c);
char    uart_rx(void);
void    uart_printstr(const char* str);

// ======== AUTH =========

# define USERNAME       "saberton"
# define PASSWORD       "42"

char    str_cmp(char *str1, char *str2);
char    get_input(char user);

// ======== LEDs =========

void    timer_led_init(void);
void    led_init(void);
void    led_loading(void);
void    res_led_blink(char res);

#endif