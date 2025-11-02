# ⏱️ MÉMO TECHNIQUE - TIMERS AVR (ATmega328P)

> Guide de référence rapide pour Timer1 (16 bits) @ 16 MHz

---

## 📌 Concepts de base

### Qu'est-ce qu'un Timer ?

Un **compteur matériel** qui s'incrémente automatiquement à chaque cycle d'horloge (ou fraction).

```
┌─────────────────────────────────────────────────┐
│  F_CPU (16 MHz)                                 │
│       ↓                                         │
│  [Prescaler /1, /8, /64, /256, /1024]           │
│       ↓                                         │
│  [Compteur TCNT1: 0 → 1 → 2 → ... → TOP]       │
│       ↓                                         │
│  [Comparaison avec OCR1A/OCR1B]                 │
│       ↓                                         │
│  [Action automatique sur pin OC1A/OC1B]         │
└─────────────────────────────────────────────────┘
```

---

## 🔧 REGISTRES PRINCIPAUX

### Vue d'ensemble

| Registre | Rôle | Contenu |
|----------|------|---------|
| **TCCR1A** | Configuration **SORTIES** | COM1A, COM1B, WGM[1:0] |
| **TCCR1B** | Configuration **COMPTEUR** | WGM[3:2], CS[2:0] (prescaler) |
| **OCR1A** | Valeur de comparaison A | Seuil pour déclencher action (16 bits) |
| **OCR1B** | Valeur de comparaison B | Seuil pour déclencher action (16 bits) |
| **ICR1** | Input Capture / TOP | Utilisé comme TOP en mode PWM 14/15 |
| **TCNT1** | Valeur actuelle | Le compteur (lecture/écriture) |

**Mnémotechnique** :
- **A** = **Actions** sur les pins (sorties)
- **B** = **Base** du timer (vitesse, mode)

---

## 🎛️ PRESCALER (CS12:10)

### Tableau des prescalers

| CS12 | CS11 | CS10 | Prescaler | F_timer @ 16MHz | Période | Durée max (16 bits) |
|------|------|------|-----------|-----------------|---------|---------------------|
| 0 | 0 | 0 | **Arrêté** | 0 Hz | ∞ | - |
| 0 | 0 | 1 | **/1** | 16 000 000 Hz | 0.0625 µs | **4 ms** |
| 0 | 1 | 0 | **/8** | 2 000 000 Hz | 0.5 µs | **33 ms** |
| 0 | 1 | 1 | **/64** | 250 000 Hz | 4 µs | **262 ms** |
| 1 | 0 | 0 | **/256** ⭐ | **62 500 Hz** | **16 µs** | **1.05 s** |
| 1 | 0 | 1 | **/1024** ⭐ | **15 625 Hz** | **64 µs** | **4.19 s** |

### Code d'activation

```c
// Prescaler /256 (CS12 = 1, CS11 = 0, CS10 = 0)
TCCR1B |= (1 << CS12);

// Prescaler /1024 (CS12 = 1, CS11 = 0, CS10 = 1)
TCCR1B |= (1 << CS12) | (1 << CS10);
```

### Comment choisir ?

```
Durée souhaitée → Choisir le plus PETIT prescaler qui permet d'atteindre cette durée

Exemple : Pour 500 ms
- /64  : max 262 ms   ❌ Trop petit
- /256 : max 1.05 s   ✅ OK !
```

---

## 🎯 MODES WGM (Waveform Generation Mode)

### Tableau complet des modes

| Mode | WGM13 | WGM12 | WGM11 | WGM10 | Nom | TOP | Usage | Toggle | PWM |
|------|-------|-------|-------|-------|-----|-----|-------|--------|-----|
| **0** | 0 | 0 | 0 | 0 | Normal | 0xFFFF | Comptage simple | ❌ | ❌ |
| **4** | 0 | 1 | 0 | 0 | **CTC** | **OCR1A** | **Fréquences précises** | ✅ | ❌ |
| **14** | 1 | 1 | 1 | 0 | **Fast PWM** | **ICR1** | **PWM duty variable** | ❌ | ✅ |
| 15 | 1 | 1 | 1 | 1 | Fast PWM | OCR1A | PWM (moins flexible) | ❌ | ✅ |

### Mode 4 - CTC (Clear Timer on Compare)

**Utilisé pour : générer des FRÉQUENCES précises (Toggle automatique)**

```c
// Configuration Mode 4 CTC
TCCR1A = (1 << COM1A0);              // Toggle OC1A
TCCR1B = (1 << WGM12) | (1 << CS12); // Mode CTC + Prescaler /256

OCR1A = 31249;  // TOP (repart à 0 quand atteint)
```

**Bits à activer** :
- `WGM12 = 1` (dans TCCR1B)
- `COM1A0 = 1` (dans TCCR1A) pour Toggle automatique

**Comportement** :
```
TCNT1:  0 ──► 31249 ──► 0 ──► 31249 ──► 0
              ↓           ↓
            Toggle      Toggle

LED:    ON ─────► OFF ─────► ON
```

**Duty cycle : TOUJOURS 50%** (car toggle)

---

### Mode 14 - Fast PWM avec ICR1

**Utilisé pour : PWM avec DUTY CYCLE VARIABLE**

```c
// Configuration Mode 14 Fast PWM
TCCR1A = (1 << COM1A1) | (1 << WGM11);                  // Clear OC1A + WGM11
TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);  // Mode 14 + /1024

ICR1 = 15624;   // TOP (définit la FRÉQUENCE)
OCR1A = 1562;   // Seuil (définit le DUTY CYCLE)
```

**Bits à activer** :
- `WGM13 = 1, WGM12 = 1, WGM11 = 1` (mode 14)
- `COM1A1 = 1` (Clear on Compare Match)

**Comportement** :
```
TCNT1:  0 ──► 1562 ──► 15624 ──► 0
        ↑      ↓         ↑
       Set   Clear      Set

LED:    HIGH ──┐        ┌── HIGH
               └────────┘
        (10%)   (90%)
```

**Duty cycle : VARIABLE** (OCR1A / ICR1 × 100%)

---

## 🔌 COM1A (Compare Output Mode)

### Tableau selon le MODE

#### En mode CTC (mode 4)

| COM1A1 | COM1A0 | Action |
|--------|--------|--------|
| 0 | 0 | Pin déconnectée |
| 0 | 1 | **Toggle OC1A** ⭐ (inverse à chaque match) |
| 1 | 0 | Clear OC1A (met à LOW) |
| 1 | 1 | Set OC1A (met à HIGH) |

**Code courant** :
```c
TCCR1A = (1 << COM1A0);  // Toggle pour fréquence 50/50
```

---

#### En mode Fast PWM (mode 14)

| COM1A1 | COM1A0 | Action |
|--------|--------|--------|
| 0 | 0 | Pin déconnectée |
| 0 | 1 | **Réservé** (ne pas utiliser) |
| 1 | 0 | **Clear on match, Set at BOTTOM** ⭐ (PWM normal) |
| 1 | 1 | Set on match, Clear at BOTTOM (PWM inversé) |

**Code courant** :
```c
TCCR1A = (1 << COM1A1);  // Clear on match (PWM non-inversé)
```

---

## 📐 FORMULES DE CALCUL

### Fréquence du Timer

```
F_timer = F_CPU / Prescaler

Exemple : 16 000 000 / 256 = 62 500 Hz
```

### Valeur OCR1A (Mode CTC pour fréquence)

```
OCR1A = (F_timer / F_désirée) - 1

Note : Si Toggle, F_désirée = 2 × F_LED (car 2 toggles = 1 cycle)

Exemple : Pour LED 1 Hz (Toggle)
F_timer = 62 500 Hz
F_désirée = 2 Hz (2 toggles)
OCR1A = (62 500 / 2) - 1 = 31 249
```

### Duty Cycle (Mode Fast PWM)

```
Duty (%) = (OCR1A / ICR1) × 100

Exemple :
OCR1A = 1562
ICR1 = 15624
Duty = (1562 / 15624) × 100 ≈ 10%
```

### Durée maximale

```
Durée_max = 65536 / F_timer

Exemple avec /256 :
Durée_max = 65536 / 62 500 ≈ 1.05 seconde
```

---

## 🎓 EXEMPLES PRATIQUES

### Exemple 1 : LED clignotante 1 Hz (50% duty)

**Consigne** : LED qui alterne 0.5s ON / 0.5s OFF

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);  // PB1 en sortie

    // Mode CTC (4), Toggle, Prescaler /256
    TCCR1A = (1 << COM1A0);              // Toggle OC1A
    TCCR1B = (1 << WGM12) | (1 << CS12); // CTC + /256

    OCR1A = 31249;  // 0.5s par toggle

    while (1) {}  // Hardware fait tout !
}
```

**Calcul** :
```
F_timer = 16 MHz / 256 = 62 500 Hz
Pour 0.5s : OCR1A = (62 500 × 0.5) - 1 = 31 249
Toggle × 2 = 1 Hz complet
```

---

### Exemple 2 : LED 1 Hz @ 10% duty cycle

**Consigne** : LED pulse 100ms ON, 900ms OFF

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);  // PB1 en sortie

    // Mode Fast PWM (14), Clear on match, Prescaler /1024
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);

    ICR1 = 15624;   // Fréquence 1 Hz
    OCR1A = 1562;   // 10% duty cycle

    while (1) {}
}
```

**Calcul** :
```
F_timer = 16 MHz / 1024 = 15 625 Hz
Pour 1 Hz : ICR1 = 15 625 / 1 - 1 ≈ 15624
Duty 10% : OCR1A = 15624 × 0.10 = 1562
```

---

### Exemple 3 : Boutons pour ajuster duty cycle

**Consigne** : SW1 incrémente, SW2 décrémente de 10%

```c
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // Pins
    DDRB |= (1 << PB1);                     // LED sortie
    DDRD &= ~((1 << PD2) | (1 << PD4));     // Boutons entrée
    PORTD |= (1 << PD2) | (1 << PD4);       // Pull-ups activés

    // Timer1 Fast PWM mode 14
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);

    ICR1 = 15624;
    uint16_t step = ICR1 / 10;  // 10%
    OCR1A = step;               // Départ 10%

    while (1)
    {
        // SW1 : Incrémenter
        if (!(PIND & (1 << PD2)))
        {
            _delay_ms(50);  // Anti-rebond
            if (!(PIND & (1 << PD2)))  // Confirmation
            {
                if (OCR1A <= ICR1 - step)
                    OCR1A += step;
                else
                    OCR1A = ICR1;  // Max 100%

                while (!(PIND & (1 << PD2)));  // Attendre relâchement
                _delay_ms(50);
            }
        }

        // SW2 : Décrémenter
        if (!(PIND & (1 << PD4)))
        {
            _delay_ms(50);
            if (!(PIND & (1 << PD4)))
            {
                if (OCR1A > step)
                    OCR1A -= step;
                // Sinon reste à 10% (minimum)

                while (!(PIND & (1 << PD4)));
                _delay_ms(50);
            }
        }
    }
}
```

---

## 🔍 ANTI-REBOND (DEBOUNCING)

### Pourquoi ?

Les boutons mécaniques ont des **rebonds** pendant 5-50ms :

```
Appui physique :
        ┌─────────────────────
        │
────────┘

Signal électrique réel :
        ┌─┐ ┌──┐  ┌──────────
        │ │ │  │  │
────────┘ └─┘  └──┘
        ↑─────────↑
        Rebonds (5-50ms)
```

### Technique de debouncing

```c
if (!(PIND & (1 << PD2)))       // 1. Détection initiale
{
    _delay_ms(50);              // 2. Attendre stabilisation (50ms)
    if (!(PIND & (1 << PD2)))   // 3. Confirmer que toujours appuyé
    {
        // Action (incrément, etc.)

        while (!(PIND & (1 << PD2)));  // 4. Attendre relâchement
        _delay_ms(50);                 // 5. Anti-rebond du relâchement
    }
}
```

**Pourquoi la double vérification ?**
- Filtre les parasites électriques
- Garantit que c'est un vrai appui stable
- Évite les faux positifs

---

## 🔌 PULL-UP

### Qu'est-ce qu'un pull-up ?

Une **résistance** qui "tire vers le haut" (VCC) le signal quand rien n'est connecté.

### Sans pull-up : ❌ PROBLÈME

```
        VCC (+5V)
           │
         [SW1]
           │
           ├──────→ PD2 (lecture)
           │
          ??? ← Pin FLOTTANTE (lecture aléatoire)
```

### Avec pull-up : ✅ STABLE

```
        VCC (+5V)
           │
           ├─── [Pull-up ~47kΩ] ← Maintient HIGH par défaut
           │                 │
         [SW1]              │
           │                │
           ├────────────────┴──→ PD2 (lecture)
           │
          GND (0V)
```

### Activation dans le code

```c
DDRD &= ~(1 << PD2);    // 1. Configurer en ENTRÉE
PORTD |= (1 << PD2);    // 2. Activer pull-up interne
```

### États

| Bouton | Circuit | PD2 |
|--------|---------|-----|
| **Relâché** | VCC → Pull-up → PD2 | **HIGH** (5V) |
| **Appuyé** | VCC → SW1 → GND | **LOW** (0V) |

### Logique de lecture

```c
if (!(PIND & (1 << PD2)))  // Si LOW → bouton appuyé
```

---

## 🎯 CHECKLIST RAPIDE

### Mode CTC (fréquence fixe 50/50)

- [ ] Pin en sortie : `DDRB |= (1 << PB1);`
- [ ] Mode CTC : `TCCR1B |= (1 << WGM12);`
- [ ] Toggle : `TCCR1A |= (1 << COM1A0);`
- [ ] Prescaler : `TCCR1B |= (1 << CS12);` (exemple /256)
- [ ] Valeur : `OCR1A = valeur;`
- [ ] Boucle vide : `while(1) {}`

### Mode Fast PWM (duty variable)

- [ ] Pin en sortie : `DDRB |= (1 << PB1);`
- [ ] Mode 14 : `WGM13=1, WGM12=1, WGM11=1, WGM10=0`
- [ ] Clear : `TCCR1A |= (1 << COM1A1);`
- [ ] Prescaler : `TCCR1B |= ...`
- [ ] TOP : `ICR1 = valeur;`
- [ ] Duty : `OCR1A = valeur;`

### Boutons avec pull-up

- [ ] Entrée : `DDRD &= ~(1 << PD2);`
- [ ] Pull-up : `PORTD |= (1 << PD2);`
- [ ] Lecture : `if (!(PIND & (1 << PD2)))`
- [ ] Anti-rebond : `_delay_ms(50);`
- [ ] Confirmation : `if (!(PIND & (1 << PD2)))`
- [ ] Attente relâchement : `while (!(PIND & (1 << PD2)));`

---

## 📊 COMPARAISON MODES CTC vs FAST PWM

| Aspect | Mode CTC (4) | Mode Fast PWM (14) |
|--------|--------------|-------------------|
| **Usage** | Générer FRÉQUENCES | Générer PWM (duty variable) |
| **TOP** | OCR1A | ICR1 |
| **Duty cycle** | Toujours 50% (toggle) | Variable (OCR1A/ICR1) |
| **COM1A** | Toggle (01) | Clear (10) |
| **Boucle while** | Vide | Vide (ou gestion boutons) |
| **Exemple** | LED 1 Hz clignotante | LED 1 Hz @ 10% duty |

---

## 💡 ERREURS COURANTES

### ❌ Oublier le prescaler
```c
TCCR1B = (1 << WGM12);  // ❌ Timer ne démarre pas !
```
✅ **Solution** : Toujours ajouter CS bits
```c
TCCR1B = (1 << WGM12) | (1 << CS12);  // ✅
```

---

### ❌ OCR1A = TOP au lieu de TOP - 1
```c
OCR1A = 31250;  // ❌ Compte de 0 à 31250 = 31251 valeurs
```
✅ **Solution** :
```c
OCR1A = 31249;  // ✅ Compte de 0 à 31249 = 31250 valeurs
```

---

### ❌ Oublier le pull-up
```c
DDRD &= ~(1 << PD2);  // Entrée configurée
// ❌ Manque PORTD |= (1 << PD2);
```
**Résultat** : Lectures aléatoires !

✅ **Solution** :
```c
PORTD |= (1 << PD2);  // ✅ Active pull-up
```

---

### ❌ Pas de confirmation après delay
```c
if (!(PIND & (1 << PD2)))
{
    _delay_ms(50);
    OCR1A += step;  // ❌ Pas de vérification !
}
```
**Résultat** : Parasites déclenchent des actions !

✅ **Solution** :
```c
if (!(PIND & (1 << PD2)))
{
    _delay_ms(50);
    if (!(PIND & (1 << PD2)))  // ✅ Confirmation
        OCR1A += step;
}
```

---

## 🔗 RÉFÉRENCES DATASHEET

| Sujet | Page |
|-------|------|
| Timer1 complet | 133-158 |
| COM1A (Compare Output Mode) | 140 |
| WGM (Waveform Generation) | 141-142 |
| CS (Clock Select / Prescaler) | 143 |
| TCCR1A détails | 139 |
| TCCR1B détails | 142 |

---

**Date** : Module01 - Piscine Embedded
**Plateforme** : ATmega328P @ 16 MHz
**Version** : Mémo simplifié et visuel
