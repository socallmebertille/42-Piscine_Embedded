#include "main.h"

void i2c_init(uint16_t kHz)         // init => Inter-Integrated Circuit | Two-Wire Interface
{
    TWSR = 0; // 0 = prescaler /1
    uint32_t scl = (uint32_t)kHz * 1000UL; // SCL = CPU_Clock / (16 + 2*TWBR*Prescaler)
    uint32_t twbr = (F_CPU / scl - 16) / 2;
    TWBR = (uint8_t)twbr;
    TWCR = (1 << TWEN); // TWEN = active TWI
}

void i2c_start(void)                // step 1:3.1 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // clear TWINT flag | génère START | active TWI
    while (!(TWCR & (1 << TWINT))); // attendre que START soit effectivement transmis                                // step 2 p.225
    
    if (STATUS != TW_START && STATUS != TW_REP_START) // vérifier la valeur des status retourner
    {
        i2c_write_status("ERROR");
        return;
    }
    else
        i2c_write_status("START");
}

void i2c_stop(void)                  // step 7.2 p.225
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // TWSTO = génère STOP
}

void i2c_write_status(char *action)
{
    // if (action) { uart_printstr(action); uart_printstr(" "); }
    // uart_printstr("status: 0x");
    // uart_print_hex(STATUS);
    // uart_printstr("\r\n");
}

void i2c_write(unsigned char data)  // step 3.2:5.1 p.225
{
    TWDR = data; // mettre le byte dans TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);  // lancer la transmission = remise des flags = clear + active TWI
    while (!(TWCR & (1 << TWINT))); // attendre la fin de la transmission

    // p.227:30 |  status possibles attendus : 
    //          TW_MT_SLA_ACK (0x18) = SLA+W transmis + slave a répondu ACK
    //          TW_MT_DATA_ACK (0x28) = Data byte transmis + slave a répondu ACK
    //          TW_MR_SLA_ACK (0x40) = SLA+R transmis + slaver a répondu ACK
    if (STATUS == TW_MT_SLA_ACK)
        i2c_write_status("SLA+W");
    else if (STATUS == TW_MT_DATA_ACK)
        i2c_write_status("WRITE");
    else if (STATUS == TW_MR_SLA_ACK)
        i2c_write_status("SLA+R");
    else
    {
        i2c_write_status("ERROR");
        return;
    }
}

float last_h[NB_MEAS] = {0};
float last_t[NB_MEAS] = {0};
uint8_t idx = 0;

float avg_h(void) {
    float sum = 0;
    for (int i = 0; i < NB_MEAS; i++) sum += last_h[i];
    return sum / NB_MEAS;
}

float avg_t(void) {
    float sum = 0;
    for (int i = 0; i < NB_MEAS; i++) sum += last_t[i];
    return sum / NB_MEAS;
}

void i2c_read(void)
{
    unsigned char data[7];

    for (uint8_t i = 0; i < 7; i++)
    {
        // p.240 |  Bit 6 – TWEA: TWI Enable Acknowledge Bit
        if (i < 6)
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // ACK => TWEA pour indiquer qu'on attend ACK sur le prochain octet reçu
        else
            TWCR = (1 << TWINT) | (1 << TWEN);               // NACK => SANS TWEA pour indiquer qu'on attend ACK sur le prochain octet reçu

        while (!(TWCR & (1 << TWINT))); // attendre la fin de transmission
        data[i] = TWDR;
    }

    // Humidité
    uint32_t humidity = ((uint32_t)data[1] << 12) |
                        ((uint32_t)data[2] << 4)  |
                        ((uint32_t)(data[3] >> 4));
    float h = (humidity * 100.0) / 1048576.0;

    // Température
    uint16_t temperature = ((uint32_t)(data[3] & 0x0F) << 16) |
                           ((uint32_t)data[4] << 8)           |
                           ((uint32_t)data[5]);
    float t = ((temperature * 200.0) / 1048576.0) - 50.0;

    last_h[idx] = h;
    last_t[idx] = t;
    idx = (idx + 1) % NB_MEAS;

    // Choix nb chiffre apres la virgule p.2 | Sensor Performance
    char buf[8];
    uart_printstr("Temperature: ");
    dtostrf(avg_t(), 4, 1, buf); // typical error entre 20 et 60C -> ±0.5
    uart_printstr(buf);
    uart_printstr("C, Humidity: ");
    dtostrf(avg_h(), 4, 0, buf); // maximum error entre 20 et 80% -> ±2
    uart_printstr(buf);
    uart_printstr("%\r\n");
}

void print_hex_value(char c)
{
    uart_print_hex((uint8_t)c);
}
