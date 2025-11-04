#include "main.h"

char is_valid_color(char c)
{
     // Convertir minuscule en majuscule
    if (c >= 'a' && c <= 'f')
        c -= 32;                             // ou c = c - 'a' + 'A'
    
    // Vérifier si c'est valide
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
        return 1;

    return 0;
}

void get_color(void)
{
    char buffer[8] = "";
    char count = 0;
    while (1)
    {
        char c = uart_rx();
        if (c == '\r' || c == '\n')          // Si touche enter
        {
            uart_printstr("\r\n");
            buffer[count] = '\0';
            get_led(buffer);                // On allume la led de la bonne couleur
            return;                         // On relance get_color(); du départ
        }
        else if (c == '\b' || c == 127)     // Si touche backspace (= DEL)
        {
            if (count > 0)                  // Seulement si il y a des caractères à effacer
            {
                uart_printstr("\b \b");
                count--;                    // Décrémente le compteur
            }
            continue;
        }
        if (count == 8 || (!count && c != '#') || (count && !is_valid_color(c)))
        {
            uart_tx(c);
            uart_printstr("\r\nThe color must be sent in <#RRGGBB> format.\r\n");
            return;                        // On relance get_color(); du départ
        }
        buffer[count++] = c;
        uart_tx(c);
    }
}