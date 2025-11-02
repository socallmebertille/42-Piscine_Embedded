#include <avr/io.h>

int main(void)
{
    // initialisation
    DDRB |= (1 << PB1);   // LED en mode sortie
    PORTB &= ~(1 << PB1); // LED OFF

    while (1)
    {
        // Inverser l'état de la LED (ON <-> OFF)
        PORTB ^= (1 << PB1);

        // Temporisation ou attente logicielle (~0.5 s)
        // Le type de variable volatile permet d'obliger 
        // le compilateur à conserver ma boucle qui a priori semble être inutile
        for (volatile unsigned int i = 0; i < 500; i++) {      // 500 ms = 0.5 sec
            for (volatile unsigned int j = 0; j < 1000; j++);  // 1000 itérations = 10ms = 0.
        }
    }
}
