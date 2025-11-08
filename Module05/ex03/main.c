#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init(void)
{
    
}

ISR()
{
    // Code à exécuter quand l'interrupt se déclenche
}

int main(void)
{
    cli(); // Ignorer les interruptions
    init();
    sei(); // Autoriser les interruptions

    while (1)
    {
        
    }
}
