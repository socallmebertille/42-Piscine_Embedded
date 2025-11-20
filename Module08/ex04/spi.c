#include "main.h"

void SPI_MasterInit(void) // p.172 |  how to initialize the SPI as a Master
{
    // mettre MOSI (= PB3), SCK (= PB5), SS (= PB2) en sortie
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
    // p.176 | SPI Control Register | SPE = SPI Enable, MSTR = Master/Slave Select, SPR0 = SCK Frequency = /16 => SCK = f_OS / prescaler = f_CPU / pres = 16MHz / 16 = 1 MHz
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData) // p.172 |  how to perform a simple transmission
{
    SPDR = cData; // start transmission
    while (!(SPSR & (1 << SPIF))); // attendre fin transmission
}

void send_action_frame(uint8_t action)
{
    for (uint8_t i = 0; i < 4; i++) {
        SPI_MasterTransmit(action);
    }
}

void led_send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    // p.1 |  premier octet luminosité formé (111 | brightness)
    if (brightness > 31) brightness = 31;

    SPI_MasterTransmit(0b11100000 | brightness);
    SPI_MasterTransmit(b);
    SPI_MasterTransmit(g);
    SPI_MasterTransmit(r);
}
