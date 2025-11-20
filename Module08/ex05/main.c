#include "main.h"

#define NB_LEDS 3
#define NB_COLORS 3

// LED index
#define D6 0
#define D7 1
#define D8 2

uint8_t leds[NB_LEDS][NB_COLORS] = {
    {0, 0, 0}, // D6
    {0, 0, 0}, // D7
    {0, 0, 0}  // D8
};

// Indices de LED et couleur en cours de modification
uint8_t current_led = 0;
uint8_t current_color = 0; // 0 = R, 1 = G, 2 = B

// Initialisation des boutons SW1 et SW2
void buttons_init(void)
{
    // SW1 = PD2, SW2 = PD3
    DDRD &= ~((1 << PD2) | (1 << PD3)); // entrées
    PORTD |= (1 << PD2) | (1 << PD4);   // pull-up
}

// Lecture boutons (1 si appuyé)
uint8_t sw1_pressed(void) { return (PIND & (1 << PD2)); }
uint8_t sw2_pressed(void) { return (PIND & (1 << PD4)); }

// Met à jour les LEDs via SPI
void update_leds(void)
{
    send_action_frame(0x00);
    for (uint8_t i = 0; i < NB_LEDS; i++)
        led_send_frame(leds[i][0], leds[i][1], leds[i][2], 1);
    send_action_frame(0xFF);
}
// Affiche une seule ligne en temps réel
void print_led_status(void)
{
    char buf[128];

    // On met en rouge la couleur modifiée
    uint8_t r = leds[current_led][0];
    uint8_t g = leds[current_led][1];
    uint8_t b = leds[current_led][2];

    sprintf(buf, "LED D%d : R=%s%03d%s G=%s%03d%s B=%s%03d%s\r",
            6 + current_led,
            current_color == 0 ? "\x1B[31m" : "", r, current_color == 0 ? "\x1B[0m" : "",
            current_color == 1 ? "\x1B[31m" : "", g, current_color == 1 ? "\x1B[0m" : "",
            current_color == 2 ? "\x1B[31m" : "", b, current_color == 2 ? "\x1B[0m" : ""
    );

    uart_printstr(buf);
}

int main(void)
{
    uart_init();
    SPI_MasterInit();
    adc_init();
    buttons_init();

    while(1)
    {
        // lecture du potentiomètre pour la couleur courante
        uint8_t val = adc_read(0);
        leds[current_led][current_color] = val;

        update_leds();
        print_led_status();

        if(!sw1_pressed()) { _delay_ms(200); current_color = (current_color+1)%NB_COLORS; }
        if(!sw2_pressed()) { _delay_ms(200); current_led = (current_led+1)%NB_LEDS; }

        _delay_ms(50);
    }
}
