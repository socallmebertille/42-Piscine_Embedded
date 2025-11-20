#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>

// ======== ADC ==========

void            adc_init(void);
uint8_t         adc_read(uint8_t peripheral);

// ======== SPI ==========

void            SPI_MasterInit(void);
void            SPI_MasterTransmit(char cData);
void            send_action_frame(uint8_t action);
void            led_send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);

#endif