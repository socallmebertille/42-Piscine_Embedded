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

