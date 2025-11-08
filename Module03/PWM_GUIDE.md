# Guide PWM AVR (ATmega328P)

## 📋 Table des matières
1. [Vue d'ensemble](#vue-densemble)
2. [Principe PWM](#principe-pwm)
3. [Modes PWM](#modes-pwm)
4. [Pins PWM disponibles](#pins-pwm-disponibles)
5. [Registres principaux](#registres-principaux)
6. [Calculs pratiques](#calculs-pratiques)
7. [Applications courantes](#applications-courantes)

---

## Vue d'ensemble

**PWM** = Pulse Width Modulation (Modulation de Largeur d'Impulsion)

### Principe visuel

```
Duty Cycle = Temps ON / Période totale

0% (LED OFF)     ┌─┐   ┌─┐   ┌─┐
                 ┘ └───┘ └───┘ └───

25%              ┌──┐  ┌──┐  ┌──┐
                 ┘  └──┘  └──┘  └──

50%              ┌────┐┌────┐┌────┐
                 ┘    └┘    └┘    └

75%              ┌──────┐──────┐──
                 ┘      └      └

100% (LED ON)    ──────────────────
```

---

## Principe PWM

### Paramètres clés

| Paramètre | Description | Formule | Exemple @ 16MHz |
|-----------|-------------|---------|-----------------|
| **Fréquence PWM** | Nombre de cycles/sec | F_CPU / (Prescaler × TOP) | 976 Hz |
| **Duty Cycle** | % temps à l'état haut | (OCRx / TOP) × 100 | 50% |
| **Résolution** | Nombre de niveaux | Dépend de TOP | 8-bit = 256 niveaux |

### Relation Fréquence ↔ Résolution

| TOP (résolution) | Fréquence PWM @ /64 | Niveaux | Usage |
|------------------|---------------------|---------|-------|
| 255 (8-bit) | 976 Hz | 256 | **⭐ LED, RGB** |
| 1023 (10-bit) | 244 Hz | 1024 | Précision |
| 255 (Phase Correct) | 488 Hz | 256 | Moteurs |

> **Trade-off** : Haute résolution → Basse fréquence, Basse résolution → Haute fréquence

---

## Modes PWM

### Tableau comparatif

| Mode | TOP | Fréquence | Avantage | Usage |
|------|-----|-----------|----------|-------|
| **Fast PWM** | ICR1 ou 0xFF/0x1FF | **Haute** | Simple, rapide | **⭐ LEDs, servos** |
| **Phase Correct PWM** | ICR1 ou 0xFF/0x1FF | Moitié de Fast PWM | Symétrique | Moteurs DC |
| **CTC + Toggle** | OCRxA | Variable | Fréquence exacte | Buzzers, horloges |

### Fast PWM vs Phase Correct

```
Fast PWM (Mode 14/15)
     ┌──┐  ┌──┐  ┌──┐
─────┘  └──┘  └──┘  └──
Compteur : 0→TOP, reset, 0→TOP...
Fréquence : F_CPU / (Prescaler × TOP)

Phase Correct PWM (Mode 10)
     ┌──┐      ┌──┐      ┌──┐
─────┘  └──────┘  └──────┘  └──
Compteur : 0→TOP→0→TOP...
Fréquence : F_CPU / (Prescaler × TOP × 2)
```

---

## Calculs pratiques

### Formule générale

```
F_PWM = F_CPU / (Prescaler × TOP)

Duty Cycle (%) = (OCRx / TOP) × 100
```

### Exemples @ F_CPU = 16 MHz

#### Fast PWM 8-bit (TOP = 255)

| Prescaler | F_PWM | Période | Visible ? |
|-----------|-------|---------|-----------|
| /1 | 62.5 kHz | 16 µs | Non |
| /8 | 7.8 kHz | 128 µs | Non |
| **/64** | **976 Hz** | **1 ms** | **Non** ⭐ |
| /256 | 244 Hz | 4 ms | Oui (scintillement) |

> **⭐ Seuil visible** : ~100 Hz (10 ms) → Utiliser > 100 Hz pour éviter scintillement
