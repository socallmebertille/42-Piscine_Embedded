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

uint8_t hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return (0);
}

uint16_t hex_to_u16(const char *s)
{
    return (hex_digit(s[0]) << 12)
         | (hex_digit(s[1]) << 8)
         | (hex_digit(s[2]) << 4)
         | hex_digit(s[3]);
}

uint8_t hex_to_u8(const char *s)
{
    return (hex_digit(s[0]) << 4) | hex_digit(s[1]);
}

void uart_readword(char *buf)
{
    char    c;
    uint8_t i = 0;

    buf[0] = 0;
    while (1)                                  // Skip blancs initiaux
    {
        c = uart_rx();
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
    }

    while (1)
    {
        if (c == ' ' || c == '\t')            // Fin de mot : espace ou fin de ligne
        {
            uart_tx(c);
            buf[i] = 0;
            return;
        }
        if (c == '\r' || c == '\n')           // ENTER '\n'
        {
            uart_printstr("\r\n");
            buf[i] = 0;
            return;
        }

        if ((c == '\b' || c == 127) && i > 0) // BACKSPACE '<-'
        {
            i--;
            uart_printstr("\b \b");
        }
        else if (c >= 32 && c <= 126)
        {
            if (i < 15)                     // sécurité taille cmd[16]
            {
                buf[i++] = c;
                uart_tx(c);
            }
        }

        buf[i] = 0;
        c = uart_rx();
    }
}

void uart_read_quoted(char *buf, uint8_t max)
{
    char    c;
    uint8_t i = 0;

    buf[0] = 0;

    do {                                  // Attendre le premier "
        c = uart_rx();
        if (c == 32)
            uart_tx(c);
    } while (c != '"');
    uart_tx('"');

    while (1)
    {
        c = uart_rx();
        if (c == '"')                    // Fin de la chaîne : deuxième "
        {
            buf[i] = 0;
            uart_tx('"');
            return;
        }
        if ((c == '\b' || c == 127))    // BACKSPACE '<-'
        {
            if (i > 0)
            {
                i--;
                uart_printstr("\b \b");
            }
            continue;
        }
        if (c == '\r' || c == '\n')     // ENTER '\n'
        {
            buf[i] = 0;
            return;
        }
        if (c >= 32 && c <= 126)
        {
            if (i < max - 1)          // sécurité taille buf[max]
            {
                buf[i++] = c;
                uart_tx(c);
            }
        }
    }
}

int find_kv(const char *key, uint16_t *addr_out)
{
    uint16_t addr = 0;
    uint8_t magic, klen, vlen;
    char tmp[40];

    while (addr < 1024)
    {
        magic = EEPROM_read(addr);
        if (magic != 0x7F)
            return -1; 

        klen = EEPROM_read(addr + 1);
        vlen = EEPROM_read(addr + 2);

        for (uint8_t i = 0; i < klen; i++)
            tmp[i] = EEPROM_read(addr + 3 + i);
        tmp[klen] = 0;

        if (ft_strcmp(tmp, key) == 0)
        {
            *addr_out = addr;
            return 0;
        }

        addr += 3 + klen + vlen;
    }

    return -1;
}

int read_kv(const char *key, char *value_out)
{
    uint16_t addr;

    if (find_kv(key, &addr) < 0)
        return -1;

    uint8_t klen = EEPROM_read(addr + 1);
    uint8_t vlen = EEPROM_read(addr + 2);

    for (uint8_t i = 0; i < vlen; i++)
        value_out[i] = EEPROM_read(addr + 3 + klen + i);

    value_out[vlen] = 0;
    return 0;
}

int write_kv(const char *key, const char *value)
{
    uint16_t addr = 0;
    uint16_t existing;

    if (find_kv(key, &existing) == 0)
        return 1; // already exists

    uint8_t klen = ft_strlen(key);
    uint8_t vlen = ft_strlen(value);
    uint16_t need = 3 + klen + vlen;

    while (addr < 1024)
    {
        if (EEPROM_read(addr) != 0x7F)
            break;
        uint8_t k = EEPROM_read(addr + 1);
        uint8_t v = EEPROM_read(addr + 2);
        addr += 3 + k + v;
    }

    if (addr + need >= 1024)
        return -2;

    EEPROM_write_if_needed(addr, 0x7F);
    EEPROM_write_if_needed(addr + 1, klen);
    EEPROM_write_if_needed(addr + 2, vlen);

    for (uint8_t i = 0; i < klen; i++)
        EEPROM_write_if_needed(addr + 3 + i, key[i]);

    for (uint8_t i = 0; i < vlen; i++)
        EEPROM_write_if_needed(addr + 3 + klen + i, value[i]);

    return addr;
}

int forget_kv(const char *key)
{
    uint16_t addr;
    if (find_kv(key, &addr) < 0)
        return -1;

    EEPROM_write_if_needed(addr, 0x00);
    return 0;
}

int main(void)
{
    uart_init();

    char cmd[16];
    char key[40];
    char value[40];
    char out[40];

    while (1)
    {
        uart_printstr("> ");
        uart_readword(cmd);

        if (ft_strcmp(cmd, "READ") == 0)         // READ
        {
            uart_read_quoted(key, 32);
            uart_printstr("\r\n");

            if (read_kv(key, out) == 0)
            {
                uart_printstr("\"");
                uart_printstr(out);
                uart_printstr("\"\r\n");
            }
            else
                uart_printstr("empty\r\n");
        }

        else if (ft_strcmp(cmd, "WRITE") == 0) // WRITE
        {
            uart_read_quoted(key, 32);
            uart_read_quoted(value, 32);
            uart_printstr("\r\n");

            int r = write_kv(key, value);

            if (r == 1) uart_printstr("already exists\r\n");
            else if (r == -2) uart_printstr("no space left\r\n");
            else uart_printstr("done\r\n");
        }

        else if (ft_strcmp(cmd, "FORGET") == 0) // FORGET
        {
            uart_read_quoted(key, 32);
            uart_printstr("\r\n");
            if (forget_kv(key) == 0) uart_printstr("done\r\n");
            else uart_printstr("not found\r\n");
        }

        else if (ft_strcmp(cmd, "PRINT") == 0) // PRINT
        {
            uart_printstr("\r\n");
            EEPROM_hexdump();
        }
    }
}
