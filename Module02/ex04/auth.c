#include "main.h"

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
