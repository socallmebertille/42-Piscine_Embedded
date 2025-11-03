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

void timer_init(void)
{
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // Active bits pour mode CTC avec prescaler /1024
    
    OCR1A = 31250; // (16 000 000 / 1024) * 2 sec

    TIMSK1 |= (1 << OCIE1A); // Active l’interruption du Timer1 sur la comparaison avec OCR1A
}

ISR(TIMER1_COMPA_vect)
{
    // Code à exécuter quand l'interrupt se déclenche
    uart_printstr("Hello World!\r\n");
}

int main(void)
{
    cli(); // Desactive les interrupts

    uart_init();
    timer_init();

    sei(); // Active les interrupts

    while (1) {}
}
