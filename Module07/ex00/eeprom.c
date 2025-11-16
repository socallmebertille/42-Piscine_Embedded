#include "main.h"

unsigned char	EEPROM_read(unsigned int uiAddress) // p.35
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

void	EEPROM_print(void)
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

/*

EEPROM (Electrically Erasable Programmable Read-Only Memory) is a non-volatile memory used to store information that must not be lost
 * when the device is no longer powered.
 * Unlike traditional read-only memories, EEPROM can be electrically erased and
 * rewritten, allowing data to be updated without removing the chip.
 * ATmega328P => EEPROM = 1024 bytes

*/