#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1); // LED en D2 en sortie

    // activation des bits du registre B pour :
    TCCR1B |= (1 << WGM12) | (1 << WGM13);   // pour timer1 mode Fast PWM car ideal pour les duty cycle
    TCCR1B |= (1 << CS10) | (1 << CS12);     // (prescaler /1024)
    
    // activation bits du registre A pour :
    TCCR1A |= (1 << COM1A1);  // datasheet p.140 : clear on compare match -> passe en low apres le seuil OCR1A
    TCCR1A |= (1 << WGM11);   // datasheet p.142 : mode 14 Fast PWM


    ICR1 = 15624;   // Pour 1 Hz : 15 625 / (15624 + 1) = 1 Hz
    OCR1A = 1562;   // 10% duty cycle : 15624 × 0.10 ≈ 1562

    while (1) {}
}
