#include "main.h"

uint8_t ft_strlen(const char *s)
{
    uint8_t i = 0;
    while (s[i])
        i++;
    return i;
}

int ft_strcmp(const char *a, const char *b)
{
    uint8_t i = 0;

    while (a[i] && b[i])
    {
        if (a[i] != b[i])
            return (a[i] - b[i]);
        i++;
    }
    return (a[i] - b[i]);
}

char is_hex(char c)
{
    if (c >= '0' && c <= '9')
        return (1);
    if (c >= 'A' && c <= 'F')
        return (1);
    if (c >= 'a' && c <= 'f')
        return (1);
    return (0);
}

char rgb_hex(char *buf)
{
    for (uint8_t i = 0; i < 6; i++)
        if (!is_hex(buf[i])) return 0;
    return 1;
}

uint8_t hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return 255;
}

uint8_t hex_to_u8(char high, char low)
{
    return (hex_digit(high) << 4) | hex_digit(low);
}

int read_line(char *buf, uint8_t max)
{
    uint8_t i = 0;
    char c;

    uart_printstr("#");                        // prompt

    while (1)
    {
        c = uart_rx();

        if (c == '\r' || c == '\n')            // fin de ligne
        {
            uart_printstr("\r\n");
            buf[i] = 0;
            return 1;
        }
        if ((c == '\b' || c == 127) && i > 0)  // backspace
        {
            i--;
            uart_printstr("\b \b");
            continue;
        }
        if (c >= 32 && c <= 126 && i < max - 1) // caractère imprimable
        {
            buf[i++] = c;
            uart_tx(c);
        }
    }
}

void apply_color(uint8_t r, uint8_t g, uint8_t b, char led)
{
    send_action_frame(0x00);

    for (uint8_t i = 0; i < 3; i++)
    {
        char target = '6' + i;  // D6, D7, D8

        if (target == led)
            led_send_frame(r, g, b, 1);
        else
            led_send_frame(0, 0, 0, 0);
    }

    send_action_frame(0xFF);
}

void wheel(uint8_t pos, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (pos < 85) {
        *r = 255 - pos * 3;
        *g = pos * 3;
        *b = 0;
    } else if (pos < 170) {
        pos -= 85;
        *r = 0;
        *g = 255 - pos * 3;
        *b = pos * 3;
    } else {
        pos -= 170;
        *r = pos * 3;
        *g = 0;
        *b = 255 - pos * 3;
    }
}

void full_rainbow(void)
{
    const uint8_t rainbow[][3] = {
        {0xFF, 0x00, 0x00}, // red
        {0xFF, 0x7F, 0x00}, // orange
        {0xFF, 0xFF, 0x00}, // yellow
        {0x00, 0xFF, 0x00}, // green
        {0x00, 0x00, 0xFF}, // blue
        {0x4B, 0x00, 0x82}, // indigo
        {0x8F, 0x00, 0xFF}  // violet
    };
    static uint8_t pos = 0;

    send_action_frame(0x00);

    // Allumer D6
    led_send_frame(rainbow[pos % 7][0], rainbow[pos % 7][1], rainbow[pos % 7][2], 1);
    // Allumer D7
    led_send_frame(rainbow[(pos + 1) % 7][0], rainbow[(pos + 1) % 7][1], rainbow[(pos + 1) % 7][2], 1);
    // Allumer D8
    led_send_frame(rainbow[(pos + 2) % 7][0], rainbow[(pos + 2) % 7][1], rainbow[(pos + 2) % 7][2], 1);

    send_action_frame(0xFF);

    pos++; // Avancer pour la prochaine itération
    if (pos >= 7) pos = 0;

    _delay_ms(250); // durée par LED
}

int main(void)
{
    uart_init();
    SPI_MasterInit();

    char buf[32];

    while (1)
    {
        if (!read_line(buf, sizeof(buf)))
            continue;

        // FULLRAINBOW
        if (!ft_strcmp(buf, "FULLRAINBOW") || !ft_strcmp(buf, "#FULLRAINBOW"))
        {
            for (uint8_t i = 0; i < 20; i++)
                full_rainbow();  // affiche et avance la couleur
            continue;
        }

        // Format doit être : #RRGGBBDX
        if (ft_strlen(buf) != 8)
        {
            uart_printstr("Invalid format. Expected #RRGGBBDX\r\n");
            continue;
        }

        if (!rgb_hex(buf))
        {
            uart_printstr("Invalid hex value.\r\n");
            continue;
        }

        uint8_t r = hex_to_u8(buf[0], buf[1]);
        uint8_t g = hex_to_u8(buf[2], buf[3]);
        uint8_t b = hex_to_u8(buf[4], buf[5]);

        if (buf[6] != 'D' || (buf[7] != '6' && buf[7] != '7' && buf[7] != '8'))
        {
            uart_printstr("Invalid LED. Expected D6, D7 or D8.\r\n");
            continue;
        }

        apply_color(r, g, b, buf[7]);
    }
}
