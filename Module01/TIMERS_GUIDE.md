# Guide Complet des Timers AVR (ATmega328P)

## 📋 Table des matières
1. [Vue d'ensemble](#vue-densemble)
2. [Timer1 - 16 bits](#timer1---16-bits)
3. [Modes de fonctionnement](#modes-de-fonctionnement)
4. [Registres principaux](#registres-principaux)
5. [Prescaler](#prescaler)
6. [Calculs pratiques](#calculs-pratiques)
7. [Exemples courants](#exemples-courants)

---

## Vue d'ensemble

L'ATmega328P dispose de **3 timers** :

| Timer | Bits | Pins OC | Utilisation typique |
|-------|------|---------|---------------------|
| **Timer0** | 8 bits | OC0A (PD6), OC0B (PD5) | Délais courts, PWM rapide |
| **Timer1** | 16 bits | OC1A (PB1), OC1B (PB2) | Délais longs, PWM précis |
| **Timer2** | 8 bits | OC2A (PB3), OC2B (PD3) | RTC, PWM audio |

> **💡 Focus du Module01** : Timer1 (16 bits) pour des temporisations précises

---

## Timer1 - 16 bits

### Caractéristiques principales

```
┌─────────────────────────────────────────────────┐
│  F_CPU (16 MHz)                                 │
│       ↓                                         │
│  [Prescaler: /1, /8, /64, /256, /1024]          │
│       ↓                                         │
│  [Compteur TCNT1: 0 → 65535]                    │
│       ↓                                         │
│  [Comparaison avec OCR1A/OCR1B]                 │
│       ↓                                         │
│  [Action sur OC1A (PB1) / OC1B (PB2)]           │
└─────────────────────────────────────────────────┘
```

### Plage de comptage

| Valeur | Description |
|--------|-------------|
| **Min** | 0x0000 (0) |
| **Max** | 0xFFFF (65 535) |
| **Type** | `uint16_t` |

---

## Modes de fonctionnement

### Tableau des modes WGM (Waveform Generation Mode)

| Mode | WGM13:0 | Nom | TOP | Update OCR1x | Description | Utilisation |
|------|---------|-----|-----|--------------|-------------|-------------|
| **0** | 0000 | Normal | 0xFFFF | Immediate | Compte jusqu'à overflow | Comptage simple |
| **4** | 0100 | **CTC** | **OCR1A** | **Immediate** | **Repart à 0 à OCR1A** | **⭐ Fréquences précises** |
| **5** | 0101 | Fast PWM (8-bit) | 0x00FF | BOTTOM | PWM 8 bits | PWM rapide |
| **14** | 1110 | Fast PWM | ICR1 | BOTTOM | PWM avec TOP variable | Servo-moteurs |
| **15** | 1111 | Fast PWM | OCR1A | BOTTOM | PWM avec TOP OCR1A | LED dimming |

> **⭐ Mode CTC (Mode 4)** : Le plus utilisé pour générer des fréquences précises !

### Configuration du mode CTC

```c
// Mode 4 : CTC avec TOP = OCR1A
TCCR1B |= (1 << WGM12);  // WGM12 = 1, WGM13 = 0
TCCR1A &= ~((1 << WGM11) | (1 << WGM10)); // WGM11 = 0, WGM10 = 0
```

---

## Registres principaux

### 🔧 TCCR1A - Timer/Counter Control Register A

**Contrôle les SORTIES (pins OC1A et OC1B)**

| Bits | Nom | Description | Valeurs |
|------|-----|-------------|---------|
| **7:6** | COM1A1:0 | Compare Output Mode A | `00` = Déconnecté<br>`01` = **Toggle OC1A** ⭐<br>`10` = Clear OC1A<br>`11` = Set OC1A |
| **5:4** | COM1B1:0 | Compare Output Mode B | Idem pour OC1B |
| **1:0** | WGM11:0 | Waveform Gen. Mode (bits bas) | Voir tableau modes |

**Exemple courant** :
```c
TCCR1A = (1 << COM1A0);  // Toggle OC1A automatiquement (PB1)
```

---

### 🔧 TCCR1B - Timer/Counter Control Register B

**Contrôle le COMPTEUR (vitesse et mode)**

| Bits | Nom | Description | Valeurs |
|------|-----|-------------|---------|
| **4:3** | WGM13:2 | Waveform Gen. Mode (bits hauts) | Voir tableau modes |
| **2:0** | CS12:0 | Clock Select (Prescaler) | `000` = Timer arrêté<br>`001` = /1 (aucun prescaler)<br>`010` = /8<br>`011` = /64<br>`100` = **/256** ⭐<br>`101` = /1024 |

**Exemple courant** :
```c
TCCR1B = (1 << WGM12) | (1 << CS12);  // Mode CTC + Prescaler /256
```

---

### 🔧 OCR1A/B - Output Compare Register

**Valeur de comparaison (16 bits)**

```c
OCR1A = 31249;  // Quand TCNT1 atteint cette valeur → action !
```

| Registre | Pin associée | Usage |
|----------|--------------|-------|
| **OCR1A** | OC1A (PB1) | LED D2 sur Arduino |
| **OCR1B** | OC1B (PB2) | LED D3 sur Arduino |

---

### 🔧 TCNT1 - Timer/Counter Register

**Valeur actuelle du compteur (16 bits, lecture/écriture)**

```c
uint16_t current_value = TCNT1;  // Lire la valeur actuelle
TCNT1 = 0;                       // Réinitialiser le compteur
```

---

## Prescaler

### Tableau des prescalers disponibles

| CS12:0 | Prescaler | Formule | Fréquence Timer @ 16MHz | Période | Durée max (16 bits) |
|--------|-----------|---------|-------------------------|---------|---------------------|
| `000` | Arrêté | - | 0 Hz | ∞ | - |
| `001` | **/1** | F_CPU / 1 | 16 000 000 Hz | 0.0625 µs | **4.096 ms** |
| `010` | **/8** | F_CPU / 8 | 2 000 000 Hz | 0.5 µs | **32.768 ms** |
| `011` | **/64** | F_CPU / 64 | 250 000 Hz | 4 µs | **262.144 ms** |
| `100` | **/256** ⭐ | F_CPU / 256 | **62 500 Hz** | **16 µs** | **1.048 s** |
| `101` | **/1024** | F_CPU / 1024 | 15 625 Hz | 64 µs | **4.194 s** |

### Comment choisir le prescaler ?

```
Durée souhaitée → Choisir le prescaler le plus petit qui permet d'atteindre cette durée

Durée max = 65536 × (1 / F_timer)
```

**Exemples** :
- **10 µs** → Prescaler /1 (max 4 ms) ✓
- **100 ms** → Prescaler /64 (max 262 ms) ✓
- **1 seconde** → Prescaler /256 (max 1.048 s) ✓
- **5 secondes** → Prescaler /1024 (max 4.194 s) ✗ Impossible sans découper !

---

## Calculs pratiques

### Formule générale

```
F_timer = F_CPU / Prescaler
OCR1A = (F_timer / F_désirée) - 1
```

### Exemples de calculs

#### 1️⃣ Générer 1 Hz (LED clignote toutes les 0.5s)

```
F_CPU = 16 000 000 Hz
Prescaler = 256
F_désirée = 2 Hz  (car toggle → 2 inversions = 1 cycle)

F_timer = 16 000 000 / 256 = 62 500 Hz
OCR1A = (62 500 / 2) - 1 = 31 249
```

**Code** :
```c
TCCR1A = (1 << COM1A0);              // Toggle OC1A
TCCR1B = (1 << WGM12) | (1 << CS12); // CTC + /256
OCR1A = 31249;                       // 0.5s
```

---

#### 2️⃣ Générer 440 Hz (Note La - buzzer)

```
F_CPU = 16 000 000 Hz
Prescaler = 64
F_désirée = 880 Hz  (toggle → 2× la fréquence)

F_timer = 16 000 000 / 64 = 250 000 Hz
OCR1A = (250 000 / 880) - 1 = 283
```

**Code** :
```c
TCCR1A = (1 << COM1A0);              // Toggle OC1A
TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC + /64
OCR1A = 283;                         // 440 Hz
```

---

#### 3️⃣ Interruption toutes les 10 ms

```
F_CPU = 16 000 000 Hz
Prescaler = 64
Période = 10 ms = 0.01 s

F_timer = 16 000 000 / 64 = 250 000 Hz
OCR1A = (250 000 × 0.01) - 1 = 2499
```

**Code** :
```c
TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC + /64
OCR1A = 2499;                        // 10 ms
TIMSK1 |= (1 << OCIE1A);             // Active l'interruption
sei();                               // Active les interruptions globales

ISR(TIMER1_COMPA_vect) {
    // Code exécuté toutes les 10 ms
}
```

---

## Exemples courants

### ⚡ Exemple 1 : LED clignotante 1 Hz (sans interruption)

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);                      // PB1 en sortie

    TCCR1A = (1 << COM1A0);                  // Toggle OC1A on Compare Match
    TCCR1B = (1 << WGM12) | (1 << CS12);     // Mode CTC, Prescaler /256
    OCR1A = 31249;                           // 0.5s → 1 Hz

    while (1) {}  // Le hardware fait tout !
}
```

**Avantages** :
- ✅ Boucle vide → CPU libre pour autres tâches
- ✅ Précision parfaite
- ✅ Pas d'interruptions

---

### ⚡ Exemple 2 : Interruption périodique 100 Hz

```c
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t counter = 0;

ISR(TIMER1_COMPA_vect)
{
    counter++;  // Incrémente toutes les 10 ms
    if (counter >= 100) {  // Toutes les secondes
        PORTB ^= (1 << PB1);  // Toggle LED
        counter = 0;
    }
}

int main(void)
{
    DDRB |= (1 << PB1);

    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // CTC, /64
    OCR1A = 2499;                        // 10 ms
    TIMSK1 |= (1 << OCIE1A);             // Active interruption Compare Match A
    sei();                               // Active interruptions globales

    while (1) {
        // Autres tâches ici
    }
}
```

**Avantages** :
- ✅ Flexibilité : code personnalisé toutes les 10 ms
- ✅ Compteur pour actions lentes
- ⚠️ Attention : ISR doit être rapide !

---

### ⚡ Exemple 3 : Buzzer musical (440 Hz - Note La)

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);  // Buzzer sur PB1

    TCCR1A = (1 << COM1A0);                           // Toggle OC1A
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // CTC, /64
    OCR1A = 283;  // 440 Hz

    while (1) {}
}
```

---

## 🎯 Checklist de configuration Timer1

### Pour générer une fréquence précise (mode Toggle)

- [ ] Configurer la pin en sortie : `DDRB |= (1 << PB1);`
- [ ] Mode CTC : `TCCR1B |= (1 << WGM12);`
- [ ] Toggle automatique : `TCCR1A |= (1 << COM1A0);`
- [ ] Choisir prescaler : `TCCR1B |= (1 << CS1x);`
- [ ] Calculer OCR1A : `OCR1A = valeur;`
- [ ] Laisser la boucle vide : `while(1) {}`

### Pour interruptions périodiques

- [ ] Mode CTC : `TCCR1B |= (1 << WGM12);`
- [ ] Choisir prescaler : `TCCR1B |= (1 << CS1x);`
- [ ] Calculer OCR1A : `OCR1A = valeur;`
- [ ] Activer interruption : `TIMSK1 |= (1 << OCIE1A);`
- [ ] Activer interruptions globales : `sei();`
- [ ] Créer ISR : `ISR(TIMER1_COMPA_vect) { ... }`

---

## 📚 Références

- **Datasheet ATmega328P** : Section 15 (Timer/Counter1)
  - p. 133-158 : Documentation complète Timer1
  - p. 140 : Tableau COM1A1:0 (Compare Output Mode)
  - p. 141 : Tableau WGM13:0 (Waveform Generation Mode)
  - p. 143 : Tableau CS12:0 (Clock Select)

- **Registres clés** :
  - `TCCR1A` : Configuration sorties (p. 139)
  - `TCCR1B` : Configuration timer (p. 142)
  - `OCR1A/B` : Valeurs de comparaison (p. 145)
  - `TIMSK1` : Masque interruptions (p. 147)

---

## 💡 Astuces et pièges

### ✅ Bonnes pratiques

1. **Toujours configurer la pin en sortie** avant d'activer COM1A
   ```c
   DDRB |= (1 << PB1);  // OBLIGATOIRE !
   ```

2. **Utiliser des variables volatile dans les ISR**
   ```c
   volatile uint8_t flag = 0;
   ```

3. **Garder les ISR courtes** (< 10 µs idéalement)

### ⚠️ Pièges courants

1. **Oublier le prescaler** → Timer ne démarre pas
   ```c
   TCCR1B = (1 << WGM12);  // ❌ Pas de CS1x → timer arrêté !
   ```

2. **Mauvais calcul OCR1A** → Fréquence incorrecte
   ```c
   OCR1A = 31250;  // ❌ Devrait être 31249 (on commence à 0)
   ```

3. **Oublier sei()** avec interruptions
   ```c
   TIMSK1 |= (1 << OCIE1A);  // Active l'interruption
   // ❌ Manque sei(); → interruption ne se déclenche jamais !
   ```

---

## 🧮 Calculateur rapide

### Formule inverse : trouver le prescaler

```
Prescaler_min = (F_CPU × Période_désirée) / 65536

Arrondir au prescaler supérieur disponible (/1, /8, /64, /256, /1024)
```

**Exemple** : pour 500 ms à 16 MHz
```
Prescaler_min = (16 000 000 × 0.5) / 65536 = 122
→ Choisir /256 (supérieur à 122)
```

---

**Date** : Module01 - Piscine Embedded
**Auteur** : Guide des Timers AVR
**Plateforme** : ATmega328P @ 16 MHz
