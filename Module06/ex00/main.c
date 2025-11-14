#include "main.h"

void i2c_init(void)     // Inter-Integrated Circuit | Two-Wire Interface
{
    // p.240:1 |  TWSR bits 1:0 = Prescaler -> les 2 premiers bits de droite sont réservés au prescaler
    TWSR = 0;           // 0 = prescaler /1 -> valeur par défaut (toutes les données de la datasheet sont en fonction de ca) + meilleure précision
    
    // p.222 |  SCL = CPU_Clock / (16 + 2*TWBR*Prescaler) = Serial Clock Line = nombre de ticks (impulsions) envoyés par seconde
    // Pour 100kHz avec 16MHz et Prescaler=1 :
    TWBR = 72;          // 100 000 = 16 000 000 / (16 + 2*TWBR*1) <=> TWBR = 72
    
    // p.239:40 |  TWCR - TWI Control Register
    TWCR = (1 << TWEN); // TWEN = 1 : Active le module TWI
}

void i2c_start(void)
{
    // p.225:6 |  A START condition is sent by writing the following value to TWCR
    // TWINT = 1 (clear  the TWINT Flag = opération TWI précédente terminée 
    //                                 + CPU peut lire statut TWSR ou lire prochaine cmd)
    // TWSTA = 1 (génère START ->  TWSTA must be written to one to transmit a START condition)
    // TWEN = 1 (active TWI -> TWEN must be set to enable the 2-wire Serial Interface)
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);              // step 1 p.225
    
    // p.221 |  attendre que TWINT repasse à 1 (indique que l'opération est terminée)
    while (!(TWCR & (1 << TWINT)));                                // step 2 p.225

    // vérifier que START a bien été transmis
    // return status values to your computer after each data transmission -> of START
    uart_printstr("START: 0x");
    uart_print_hex(TWSR & 0xF8);
    uart_printstr("\r\n");
    
    // p.227 |  status attendus : TW_START (0x08) ou TW_REP_START (0x10) (version macro de twi.h)
    // on check la valeur du registre sans les bits reserve au prescaler (0xF8 = 1111 1000)
    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START) // step 3.1 p.225
    {
        uart_printstr("ERROR: START failed\r\n");
        return;
    }

    // https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf?utm_source=chatgpt.com
    // -> p.8 |  the 7-bit I2C device address 0x38
    // p.241 |  TWDR – TWI Data Register
    // envoyer l'adresse du slave (AHT20 = 0x38) en mode écriture
    // SLA+W = adresse du périphérique esclave + bit W = write = 0 = le maître veut écrire vers l’esclave
    TWDR = (AHT20_ADDR << 1) | 0;                                   // step 3.2 p.225
    TWCR = (1 << TWINT) | (1 << TWEN);  // remise des flags = clear + active TWI
    // p.225 |  attendre que TWINT repasse à 1 (=> SLA+W transmis + ACK/NACK recu)
    while (!(TWCR & (1 << TWINT)));                                 // step 4 p.225

    // vérifier que le slave a répondu ACK
    // return status values to your computer after each data transmission -> of SLA+W
    uart_printstr("SLA+W: 0x");
    uart_print_hex(TWSR & 0xF8);
    uart_printstr("\r\n");
    
    // p.227 |  status attendus: TW_MT_SLA_ACK (0x18) = SLA+W transmis + slave a répondu ACK (version macro de twi.h)
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)                             // step 5.1 p.225
    {
        uart_printstr("ERROR: Slave NACK\r\n");
        return;
    }

    // on prépare pas la réception données capteur/slave            // step 5.2 p.225
    // on n'attend la fin de la reception                           // step 6 p.225
    // on verifie pas le status de retour attendu                   // step 7.1 p.225
    // car pas demander dans le sujet
}

void i2c_stop(void)
{
    // p.224:7 |  Pour générer une condition STOP
    // TWSTO = 1 (génère STOP)
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);                 // step 7.2 p.225

    uart_printstr("STOP\r\n");
}

int main(void)
{
    uart_init();
    i2c_init();

    uart_printstr("\r\n=== I2C Test ===\r\n");

    i2c_start();
    i2c_stop();

    while (1) {}
}

/*

Sortie attendue sur UART :
=== I2C Test ===
    START: 0x08
    SLA+W: 0x18
    STOP

Ou si le capteur ne répond pas :
    === I2C Test ===
    START: 0x08
    SLA+W: 0x20
    ERROR: Slave NACK
    STOP

Code de status :
    0x08 = TW_START (START transmis)
    0x18 = TW_MT_SLA_ACK (Slave a répondu ACK)
    0x20 = TW_MT_SLA_NACK (Slave n'a pas répondu)

*/