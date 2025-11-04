# Guide UART AVR (ATmega328P)

## 📋 Table des matières
1. [Vue d'ensemble](#vue-densemble)
2. [Configuration UART](#configuration-uart)
3. [Registres principaux](#registres-principaux)
4. [Transmission et réception](#transmission-et-réception)
5. [Interrupts UART](#interrupts-uart)
6. [Calculs pratiques](#calculs-pratiques)

---

## Vue d'ensemble

**UART** = Universal Asynchronous Receiver-Transmitter

### Caractéristiques principales

```
┌─────────────────────────────────────────────────┐
│  MCU (ATmega328P)           PC (Terminal)       │
│       ↓                          ↑              │
│    TX (PD1) ────────────────→ RX               │
│    RX (PD0) ←──────────────── TX               │
│    GND      ←──────────────── GND              │
└─────────────────────────────────────────────────┘
```

| Pin | Fonction | Arduino | Description |
|-----|----------|---------|-------------|
| **PD0** | RXD | D0 | Réception de données |
| **PD1** | TXD | D1 | Transmission de données |

> **⚠️ Attention** : TX du MCU → RX du PC (croisé !)

---

## Configuration UART

### Format de trame : 8N1

```
    Start   D0  D1  D2  D3  D4  D5  D6  D7  Stop
     ↓      ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓    ↓
   ┌───┐┌───┬───┬───┬───┬───┬───┬───┬───┬───┐
   │ 0 ││ 1 │ 0 │ 1 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │  Exemple : 'A' (0x41)
   └───┘└───┴───┴───┴───┴───┴───┴───┴───┴───┘
```

| Format | Description | Valeur typique |
|--------|-------------|----------------|
| **8** | 8 bits de données | 1 octet |
| **N** | No parity (pas de parité) | Pas de vérification d'erreur |
| **1** | 1 stop bit | Fin de trame |

### Baudrates courants

| Baudrate | Utilisation | UBRR @ 16MHz |
|----------|-------------|--------------|
| **9600** | Débogage lent, faible bruit | 103 |
| **19200** | Standard | 51 |
| **38400** | Standard | 25 |
| **57600** | Rapide | 16 |
| **115200** ⭐ | **Très rapide** | **8** |

> **⭐ 115200 bauds** : Standard moderne pour communication rapide

---

## Registres principaux

### 🔧 UBRRn - UART Baud Rate Register (16 bits)

**Définit la vitesse de communication**

```c
#define BAUD 115200
#define UART_BAUDRATE (F_CPU / (16 * BAUD))

UBRR0H = (unsigned char)(UART_BAUDRATE >> 8);  // 8 bits hauts
UBRR0L = (unsigned char)UART_BAUDRATE;         // 8 bits bas
```

| Registre | Bits | Description |
|----------|------|-------------|
| **UBRR0H** | 15:8 | Partie haute du baudrate |
| **UBRR0L** | 7:0 | Partie basse du baudrate |

---

### 🔧 UCSRnA - UART Control and Status Register A

**État de la transmission/réception**

| Bit | Nom | Description | Lecture | Utilisation |
|-----|-----|-------------|---------|-------------|
| **7** | RXC0 | **RX Complete** | 1 = Donnée reçue | Polling RX |
| **6** | TXC0 | TX Complete | 1 = Transmission terminée | Vérifier fin TX |
| **5** | UDRE0 | **Data Register Empty** | 1 = Buffer TX vide | **⭐ Polling TX** |
| **4** | FE0 | Frame Error | 1 = Erreur de trame | Détection d'erreur |
| **3** | DOR0 | Data OverRun | 1 = Donnée perdue | Buffer overflow |
| **2** | UPE0 | Parity Error | 1 = Erreur de parité | Si parité activée |


---

### 🔧 UCSRnB - UART Control and Status Register B

**Active/désactive RX, TX et interrupts**

| Bit | Nom | Description | Valeur |
|-----|-----|-------------|--------|
| **7** | RXCIE0 | RX Complete Interrupt Enable | 1 = Active interrupt RX |
| **6** | TXCIE0 | TX Complete Interrupt Enable | 1 = Active interrupt TX |
| **5** | UDRIE0 | Data Register Empty Interrupt | 1 = Active interrupt buffer vide |
| **4** | RXEN0 | **Receiver Enable** | **1 = Active RX** ⭐ |
| **3** | TXEN0 | **Transmitter Enable** | **1 = Active TX** ⭐ |
| **2** | UCSZ02 | Character Size bit 2 | Voir tableau format |


---

### 🔧 UCSRnC - UART Control and Status Register C

**Configure le format de trame**

| Bits | Nom | Description | Valeurs |
|------|-----|-------------|---------|
| **6** | UMSEL01:0 | Mode Select | `00` = UART asynchrone ⭐ |
| **5:4** | UPM01:0 | Parity Mode | `00` = No parity ⭐<br>`10` = Even parity<br>`11` = Odd parity |
| **3** | USBS0 | Stop Bit Select | `0` = 1 stop bit ⭐<br>`1` = 2 stop bits |
| **2:1** | UCSZ01:0 | Character Size | `11` = 8 bits ⭐ |


---

### 🔧 UDRn - UART Data Register

**Buffer de données (lecture/écriture)**

| Opération | Description |
|-----------|-------------|
| **Écriture** | Place un caractère dans le buffer TX |
| **Lecture** | Lit le caractère reçu du buffer RX |

> **⚠️ Important** : UDR0 est partagé entre RX et TX, mais géré automatiquement par le hardware !

---

## Transmission et réception

### Méthode Polling

**Transmission** : Attendre que `UDRE0 = 1` (buffer vide) puis écrire dans `UDR0`

**Réception** : Attendre que `RXC0 = 1` (caractère reçu) puis lire `UDR0`

---

## Interrupts UART

### Vecteurs d'interrupts disponibles

| Vecteur | Condition | Usage typique |
|---------|-----------|---------------|
| **USART_RX_vect** | Caractère reçu (RXC0 = 1) | **⭐ Echo, parsing de commandes** |
| **USART_UDRE_vect** | Buffer TX vide (UDRE0 = 1) | Transmission asynchrone |
| **USART_TX_vect** | Transmission terminée (TXC0 = 1) | Désactiver driver RS-485 |

### Avantages des interrupts

- Programme principal libre pendant l'attente
- Pas de polling actif (économie CPU)
- Réactivité immédiate

---

## Calculs pratiques

### Formule du baudrate

```
UBRR = (F_CPU / (16 × Baudrate)) - 1

Pour mode asynchrone normal (U2X = 0)
```

### Tableau de calcul @ F_CPU = 16 MHz

| Baudrate | Formule | UBRR (décimal) | UBRR (hex) | Erreur (%) |
|----------|---------|----------------|------------|------------|
| **9600** | 16M/(16×9600)-1 | 103 | 0x67 | 0.2% |
| **19200** | 16M/(16×19200)-1 | 51 | 0x33 | 0.2% |
| **38400** | 16M/(16×38400)-1 | 25 | 0x19 | 0.2% |
| **57600** | 16M/(16×57600)-1 | 16 | 0x10 | 2.1% |
| **115200** | 16M/(16×115200)-1 | **8** | **0x08** | **3.5%** ⚠️ |

> **⚠️ 115200 bauds** : Taux d'erreur de 3.5% acceptable mais limite haute

### Mode double vitesse (U2X = 1)

Activer le bit `U2X0` dans `UCSR0A` pour réduire l'erreur de baudrate.

**Formule devient :** `UBRR = (F_CPU / (8 × Baudrate)) - 1`

| Baudrate | UBRR (U2X=1) | Erreur (%) |
|----------|--------------|------------|
| **115200** | 16 | **2.1%** ✓ |

---

## Résumé des bits essentiels

| Opération | Registre | Bits à activer |
|-----------|----------|----------------|
| **Activer TX** | UCSR0B | `TXEN0` |
| **Activer RX** | UCSR0B | `RXEN0` |
| **Format 8N1** | UCSR0C | `UCSZ01` + `UCSZ00` |
| **Attendre buffer TX vide** | UCSR0A | Tester `UDRE0` |
| **Attendre réception** | UCSR0A | Tester `RXC0` |
| **Interrupt RX** | UCSR0B | `RXCIE0` + `sei()` |
