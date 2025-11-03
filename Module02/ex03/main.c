#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200
#define UART_BAUDRATE (F_CPU / (16 * BAUD))

void uart_init(void)
{
    // Initialise vitesse d'echantillonnage et de transmission
    UBRR0H = (unsigned char)(UART_BAUDRATE >> 8); // 8 bits haut du registre de baudrate du module UART
    UBRR0L = (unsigned char)UART_BAUDRATE;      // 8 bits bas
    // Active reception, transmission et interruption de reception
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
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

ISR(USART_RX_vect)
{
    // Code à exécuter quand l'interrupt se déclenche
    char c = UDR0;
    if (c == '\r' || c == '\n') // quand screen recoit "enter" -> '\r'
    {
        uart_tx('\r');
        uart_tx('\n');
    }
    else
    {
        if (c >= 'a' && c <= 'z') c -= 32; // si lower -> upper
        else if (c >= 'A' && c <= 'Z') c += 32; // si upper -> lower
        uart_tx(c);
    }
}

int main(void)
{
    cli();
    uart_init();
    sei();

    while (1) {}
}
