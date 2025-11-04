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