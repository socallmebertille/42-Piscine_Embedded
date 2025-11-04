#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200
#define UART_BAUDRATE (F_CPU / (16 * BAUD))

#define USERNAME "saberton"
#define PASSWORD "42"

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

// ================= PARSING =====================

char str_cmp(char *str1, char *str2)
{
    char i = 0;
    while (str1[i] && str2[i])
    {
        if (str1[i] != str2[i])
            return 0;
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0')
        return 1;
    return 0;
}

char get_input(char user)
{
    char buffer[42] = "";
    char count = 0;
    if (user == 1)
        uart_printstr("\tusername: ");
    else if (!user)
        uart_printstr("\tpassword: ");
    while (1)
    {
        char c = uart_rx();
        if (c == '\r' || c == '\n') // Si touche enter
        {
            uart_printstr("\r\n");
            buffer[count] = '\0';
            if ((user == 1 && str_cmp(buffer, USERNAME)) || (!user && str_cmp(buffer, PASSWORD)))
                return 1;
            else
                return 0;
        }
        else if (c == '\b' || c == 127) // Si touche backspace (= DEL)
        {
            if (count > 0) // Seulement si il y a des caractères à effacer
            {
                uart_printstr("\b \b");
                count--; // Décrémente le compteur
            }
            continue;
        }
        if (count == 42)
        {
            if (user == 1)
                uart_printstr("\r\nThe maximum length of the username is 41 characters.\r\n");
            else if (!user)
                uart_printstr("\r\nThe maximum length of the password is 41 characters.\r\n");
            return 0;
        }
        buffer[count++] = c;
        if (user == 1)
            uart_tx(c);
        else if (!user)
            uart_tx('*');
    }
}

// ===============================================

int main(void)
{
    cli();
    uart_init();
    sei();

    char step = 1;
    while (1)
    {
        if (step == 1)
        {
            uart_printstr("Enter your login:\r\n");
            char res = get_input(1) & get_input(0);
            if (res)
            {
                uart_printstr("Hello saberton!\n\r");
                uart_printstr("Shall we play a game?\r\n");
                step++;
            }
            else
            {
                uart_printstr("Bad combination username/password\r\n");
            }
        }
    }
}
