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
        for (volatile unsigned int i = 0; i < 500; i++) {      // 500 * 1 ms = 500 * 0.001 sec = 0.5 sec
            for (volatile unsigned int j = 0; j < 842; j++);  // 1000 itérations = 1ms = 0.001 sec
        }
    }
}

/*

 ● λ avr-objdump -d main.bin/           // commande qui affiche le code en ASM

Instructions a regarder :                                              Nb de cycles
  aa:   89 81           ldd     r24, Y+1        ; 0x01                          2
  ac:   9a 81           ldd     r25, Y+2        ; 0x02                          2
  ae:   88 3e           cpi     r24, 0xE8       ; 232                           1
  b0:   93 40           sbci    r25, 0x03       ; 3                             1
  b2:   30 f4           brcc    .+12            ; 0xc0 <main+0x40>              1
  b4:   89 81           ldd     r24, Y+1        ; 0x01                          2
  b6:   9a 81           ldd     r25, Y+2        ; 0x02                          2
  b8:   01 96           adiw    r24, 0x01       ; 1                             2
  ba:   9a 83           std     Y+2, r25        ; 0x02                          2
  bc:   89 83           std     Y+1, r24        ; 0x01                          2
  be:   f5 cf           rjmp    .-22            ; 0xaa <main+0x2a>              2
                                                                        TOTAL = 19

        16 MHz = 16 000 000 Hz = 16 000 000 cycles par seconde
 ________________________________________________________________________________
|Pour calculer combien de cycles dans 1 ms : 1 ms = 1/1000 seconde = 0.001 sec   |
|Nombre de cycles en : 1 ms = 16 000 000 cycles/s × 0.001 s                      |
|                           = 16 000 cycles                                      |
|________________________________________________________________________________|

ATmega328P tourne à 16 MHz donc on a : 16 000 cycles = 1 ms
Ici, on a donc : 19 cycles * 842 iterations = 15 998 cycles ≈ 1.00 ms

*/