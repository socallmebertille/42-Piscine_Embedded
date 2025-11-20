#include "main.h"

void uart_init(void)
{
    // Initialise vitesse d'echantillonnage et de transmission
    UBRR0H = (unsigned char)(UART_BAUDRATE >> 8); // 8 bits haut du registre de baudrate du module UART
    UBRR0L = (unsigned char)UART_BAUDRATE;      // 8 bits bas
    // Active reception et transmission
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Initialise format, ici 8N1, ex envoie 'A' -> start 0100 0001 stop
    //                                              0     0100 0001 1
    // 8 bits, aucun bit de parité (pas de vérification d’erreur), 1 stop bit de fin de trame
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(char c)
{
    // Attendre tant que buffer vide
    while ( !( UCSR0A & (1 << UDRE0)) )
        ;
    // Mettre la lettre dans le buffer, envoyer la donnee
    UDR0 = c;
}

void uart_print_hex(uint8_t val)
{
    const char hex[] = "0123456789ABCDEF";
    uart_tx(hex[val >> 4]);
    uart_tx(hex[val & 0x0F]);
}

void uart_print_nbr(uint32_t val)
{
    if (val >= 10)
        uart_print_nbr(val / 10);
    uart_tx('0' + (val % 10));
}

void uart_printstr(const char* str)
{
    while (*str)
        uart_tx(*str++);
}

char uart_rx(void)
{
    // Attend de recevoir de la donnee
    while ( !(UCSR0A & (1 << RXC0)) )
        ;
    // Recoit et renvoie la donnee dans le buffer
    return UDR0;
}

void uart_nibble(uint8_t n)
{
	const char	*hex = "0123456789abcdef";

	uart_tx(hex[n & 0xF]);
}

void uart_addr(unsigned int addr)
{
	uart_printstr("000");
	uart_nibble((addr >> 12) & 0xF);
	uart_nibble((addr >> 8) & 0xF);
	uart_nibble((addr >> 4) & 0xF);
	uart_nibble(addr & 0xF);
	uart_printstr(" ");
}
