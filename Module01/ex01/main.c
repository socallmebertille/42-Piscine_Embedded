#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);     // PB1 (OC1A) configuré en sortie pour la LED D2

    // TCCR1A (Timer/Counter Control Register 1 A)
    // COM1A0 = 1 : Active le mode Toggle OC1A (inverse automatiquement PB1 à chaque comparaison)
    TCCR1A = (1 << COM1A0); // datasheet : Toggle OC1A/OC1B on Compare Match. p.140

    // TCCR1B (Timer/Counter Control Register 1 B)
    // WGM12 = 1 : Mode CTC (Clear Timer on Compare) - remet le compteur à 0 automatiquement quand il atteint OCR1A
    // CS12 = 1 : Prescaler /256 pour ralentir le timer (divise 16 MHz par 256 = 62 500 Hz)
    TCCR1B = (1 << WGM12) | (1 << CS12); // datasheet : mode 4 CTC p.141 | prescaler /256 p.143

    // OCR1A (Output Compare Register 1 A)
    // Valeur de comparaison : 31 249 (car le compteur commence à 0)
    // Calcul : 16 000 000 Hz / 256 / 2 Hz = 31 250 - 1 = 31 249
    // Le timer compte de 0 à 31 249 (31 250 valeurs) puis inverse PB1 → période de 0.5s
    // Deux inversions = 1 cycle complet = 1 Hz
    OCR1A = 31249;

    while (1) {} // boucle vide du programme
}

/*

F_CPU = 16 MHz
Pour 1 Hz : LED ON pendant 0.5s, LED OFF pendant 0.5s
Donc interruption toutes les 0.5s

Avec prescaler /256 :
Timer_freq = 16 000 000 / 256 = 62 500 Hz
Pour 0.5s : 62 500 × 0.5 = 31 250 ticks

De plus : 31 250 < 65 535 (max 16 bits pour Timer1)

*/