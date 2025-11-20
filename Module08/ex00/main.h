#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>

// ======== SPI ==========

void            SPI_MasterInit(void);
void            SPI_MasterTransmit(char cData);
void            send_action_frame(uint8_t action);
void            led_send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);

#endif