#include "main.h"

void i2c_init(void)     // Inter-Integrated Circuit | Two-Wire Interface
{
    // p.222 |  SCL = CPU_Clock / (16 + 2*TWBR*Prescaler)
    // Pour 100kHz avec 16MHz et Prescaler=1 :
    TWBR = 72;          // 100 000 = 16 000 000 / (16 + 2*TWBR*1) <=> TWBR = 72
    
    // p.241 |  TWSR bits 1:0 = Prescaler
    TWSR = 0;           // 0 = prescaler /1 -> Valeur par défaut → meilleure précision
    
    // p.239.40 |  TWCR - TWI Control Register
    TWCR = (1 << TWEN); // TWEN = 1 : Active le module TWI
}

void i2c_start(void)
{
    // p.223 |  Pour générer une condition START
    // TWINT = 1 (clear interrupt flag = opération TWI précédente terminée 
    //                                 + CPU peut lire statut TWSR ou lire prochaine cmd)
    // TWSTA = 1 (génère START)
    // TWEN = 1 (active TWI)
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // p.221 |  Attendre que TWINT repasse à 1
    // (indique que l'opération est terminée)
    while (!(TWCR & (1 << TWINT)));
    
    // Optionnel : vérifier le code de statut
    // p.226 |  Table 21-3 - Status codes
    // START transmis : 0x08
    // Repeated START : 0x10
}

void i2c_stop(void)
{
    // p.224 |  Pour générer une condition STOP
    // TWINT = 1 (clear interrupt flag)
    // TWSTO = 1 (génère STOP)
    // TWEN = 1 (active TWI)
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    
    // Note : Pas besoin d'attendre TWINT pour STOP
    // Le STOP est automatique
}

int main(void)
{
    i2c_init();
    i2c_start();
    // Envoyer adresse 0x38 en mode écriture : (0x38 << 1) | 0
    i2c_stop();

    while (1)
    {

    }
}
