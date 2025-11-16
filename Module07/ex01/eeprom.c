#include "main.h"

unsigned char EEPROM_read(unsigned int uiAddress) // p.35
{
    /* Wait for completion of previous write */
	while (EECR & (1 << EEPE)); // p.31:2 | EECR = Control Register | EEPE = Write Enable bit
    
	/* Set up address register */
	EEAR = uiAddress;           // p.31 | Address Register
    
    /* Start eeprom read by writing EERE */
	EECR |= (1 << EERE);
    
    /* Return data from Data Register */
	return (EEDR);              // p.31 | Data Register
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData) // p.34
{
	/* Wait for completion of previous write */
	while (EECR & (1 << EEPE));

	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;

    /* Write logical one to EEMPE */
	EECR |= (1 << EEMPE); // p.32 | Master Write Enable

    /* Start eeprom write by setting EEPE */
	EECR |= (1 << EEPE);
}

void EEPROM_print(void)
{
    unsigned int	addr;
    
	addr = 0;
	while (addr < 1024) // p.31 | 1Kbytes EEPROM space .. EEPROM data bytes are addressed linearly between 0 and 1023
	{
        uart_addr(addr);
		for (int i = 0; i < 16; i += 2)
		{
            uart_print_hex(EEPROM_read(addr + i));
			uart_print_hex(EEPROM_read(addr + i + 1));
			uart_printstr(" ");
		}
		uart_printstr("\r\n");
		addr += 16;
	}
}

int16_t EEPROM_write_if_needed(uint16_t addr, uint8_t value)
{
    if (EEPROM_read(addr) != value)
    {
        EEPROM_write(addr, value);
        return addr;       // highlight cette adresse
    }
    return -1;             // rien écrit → pas de highlight
}

void EEPROM_highlight(uint16_t highlight)
{
    for (uint16_t base = 0; base < 1024; base += 16)
    {
        uart_addr(base);

        for (uint8_t i = 0; i < 16; i += 2)
        {
            uint16_t a1 = base + i;
            uint16_t a2 = base + i + 1;

            uint8_t b1 = EEPROM_read(a1);
            uint8_t b2 = EEPROM_read(a2);

            if (a1 == highlight)           // first octet
                uart_printstr("\033[31m"); // rouge
            uart_print_hex(b1);
            if (a1 == highlight)
                uart_printstr("\033[0m");

            if (a2 == highlight)           // second octect
                uart_printstr("\033[31m");
            uart_print_hex(b2);
            if (a2 == highlight)
                uart_printstr("\033[0m");

            uart_printstr(" ");
        }

        uart_printstr("\r\n");
    }
    uart_printstr("\r\n");
}
