#include "main.h"

void SPI_MasterInit(void) // p.172 |  how to initialize the SPI as a Master
{
    /* Set MOSI and SCK output, all others input */
    // mettre MOSI (= PB3), SCK (= PB5), SS (= PB2) en sortie
    // MOSI (Master Out Slave In) -> donnees sont sortantes
    // SCK (Serial Clock) -> signal piloter par master
    // SS / CS (Slave Select / Chip Select) -> master active ou désactive les esclaves en tirant leur SS à LOW (0)
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);

    /* Enable SPI, Master, set clock rate fck/16 */
    // p.176 | SPI Control Register
    // SPE = SPI Enable, MSTR = Master/Slave Select, SPR0 = SCK Frequency = /16 => SCK = f_OS / prescaler = f_CPU / pres = 16MHz / 16 = 1 MHz
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData) // p.172 |  how to perform a simple transmission
{
    /* Start transmission */
    // p.178 |  SPI Data Register
    SPDR = cData;
    /* Wait for transmission complete */
    // p.177 |  SPSR = SPI Status Register, SPIF = SPI Interrupt Flag
    while (!(SPSR & (1 << SPIF))); // transmission est terminée = bit SPIF à 1 dans le registre SPSR
}

void send_action_frame(uint8_t action)
{
    for (uint8_t i = 0; i < 4; i++) {
        SPI_MasterTransmit(action);
    }
}

void led_send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    // p.1 |  56 gray scale and 32 brightness adjustment
    // premier octet luminosité formé de
    //     3 bits 111
    //     5 bits global de 0-31 car 2^5 = 32
    if (brightness > 31) brightness = 31;

    SPI_MasterTransmit(0b1110000 | brightness);
    SPI_MasterTransmit(b);
    SPI_MasterTransmit(g);
    SPI_MasterTransmit(r);
}
