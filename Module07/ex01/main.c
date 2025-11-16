#include "main.h"

uint8_t	hex_digit(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	return (0);
}

uint16_t	hex_to_u16(const char *s)
{
	return (hex_digit(s[0]) << 12) | (hex_digit(s[1]) << 8) | (hex_digit(s[2]) << 4) | hex_digit(s[3]);
}

uint8_t	hex_to_u8(const char *s)
{
	return (hex_digit(s[0]) << 4) | hex_digit(s[1]);
}

char *read_addr(char *addr)
{
    char buffer[8];
    int  idx;
    char c;
    char x;
    uint16_t a;

    int address_ok = 0;

    while (!address_ok)
    {
        uart_printstr("Address [0000–03FF]: ");
        idx = 0;

        while (1)
        {
            c = uart_rx();
            if (c == '\b' || c == 127)            // BACKSPACE '<-'
            {
                if (idx > 0)
                {
                    idx--;
                    uart_printstr("\b \b");
                }
                continue;
            }
            if (c == '\r' || c == '\n')           // ENTER '\n'
            {
                buffer[idx] = 0;
                uart_printstr("\r\n");

                if (idx != 4)
                {
                    uart_printstr("Invalid: must be 4 hex digits.\r\n");
                    break; // on laisse la boucle externe recommencer
                }
                int valid = 1;
                for (int i = 0; i < 4; i++)       // valid hexa format
                {
                    x = buffer[i];
                    if (!((x >= '0' && x <= '9') ||
                          (x >= 'A' && x <= 'F') ||
                          (x >= 'a' && x <= 'f')))
                    {
                        valid = 0;
                        break;
                    }
                }

                if (!valid)
                {
                    uart_printstr("Invalid: only 0-9 A-F allowed.\r\n");
                    break;
                }
                a = hex_to_u16(buffer);         // conversion to check if overflow addr
                if (a > 0x03FF)
                {
                    uart_printstr("Invalid: out of range (0000–03FF).\r\n");
                    break;
                }
                for (int i = 0; i < 4; i++)     // put addr into own buf
                    addr[i] = buffer[i];
                addr[4] = 0;
                return (addr);
            }
            if (idx < 4 &&                      // check for hexa val
                ((c >= '0' && c <= '9') ||
                 (c >= 'A' && c <= 'F') ||
                 (c >= 'a' && c <= 'f')))
            {
                buffer[idx++] = c;
                uart_tx(c);
            }
        }
    }
}

char *read_val(char *val)
{
    char buffer[8];
    int  idx;
    char c;
    char x;
    int value_ok = 0;

    while (!value_ok)
    {
        uart_printstr("Value   [00–FF]: ");
        idx = 0;

        while (1)
        {
            c = uart_rx();
            if (c == '\b' || c == 127)            // BACKSPACE '<-'
            {
                if (idx > 0)
                {
                    idx--;
                    uart_printstr("\b \b");
                }
                continue;
            }
            if (c == '\r' || c == '\n')           // ENTER '\n'
            {
                buffer[idx] = 0;
                uart_printstr("\r\n");

                if (idx != 2)
                {
                    uart_printstr("Invalid: must be 2 hex digits.\r\n");
                    break;
                }
                int valid = 1;
                for (int i = 0; i < 2; i++)       // valid hexa format
                {
                    x = buffer[i];
                    if (!((x >= '0' && x <= '9') ||
                          (x >= 'A' && x <= 'F') ||
                          (x >= 'a' && x <= 'f')))
                    {
                        valid = 0;
                        break;
                    }
                }
                if (!valid)
                {
                    uart_printstr("Invalid: only 0-9 A-F allowed.\r\n");
                    break;
                }
                val[0] = buffer[0];             // put val into own buf
                val[1] = buffer[1];
                val[2] = 0;
                return (val);
            }
            if (idx < 2 &&                      // check for hexa val
                ((c >= '0' && c <= '9') ||
                 (c >= 'A' && c <= 'F') ||
                 (c >= 'a' && c <= 'f')))
            {
                buffer[idx++] = c;
                uart_tx(c);
            }
        }
    }
}

int	main(void)
{
	char		addr_str[5];
	char		val_str[3];
	uint16_t	addr;
	uint8_t		value;
    int16_t     changed;

	uart_init();

	while (1)
	{
		addr = hex_to_u16(read_addr(addr_str));
		value = hex_to_u8(read_val(val_str));
		changed = EEPROM_write_if_needed(addr, value);
        EEPROM_highlight(changed);
	}
	return (0);
}
