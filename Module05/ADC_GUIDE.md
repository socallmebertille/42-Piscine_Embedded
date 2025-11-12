# L’ADC (Analog-to-Digital Converter)
    
Un **ADC** (pour *Analog-to-Digital Converter*, ou **convertisseur analogique–numérique** en français) est un **composant électronique** qui transforme un **signal analogique** (continu) en un **signal numérique** (discret) → 0 à 1023 pour un ADC 10 bits

C’est ce qui permet au microcontrôleur de *comprendre* les signaux du monde réel tels que la lumière d’un capteur, la température, le son, etc.
    
## Fonctionnement

1. **Échantillonnage (sampling)** :
    
    Le signal analogique est « mesuré » à intervalles réguliers (ex. 1000 fois par seconde → 1 kHz).
    
2. **Quantification** :
    
    Chaque mesure est **arrondie** à la valeur numérique la plus proche (selon la résolution du convertisseur).
    
3. **Codage binaire** :
    
    La valeur quantifiée est convertie en une suite de bits.
            
## Paramètres
        
| Paramètre | Description |
| --- | --- |
| **Résolution (en bits)** | Détermine la précision. Un ADC 8 bits → 256 valeurs possibles (0–255). Un ADC 12 bits → 4096 valeurs (0–4095). |
| **Tension de référence (Vref)** | Définit la plage de tension mesurable (ex. 0–5 V ou 0–3.3 V). |
| **Fréquence d’échantillonnage** | Nombre de mesures par seconde. Plus elle est élevée, plus le signal est fidèlement représenté. |

## Référence ADC
        
| Référence | Plage de mesure | Usage |
| --- | --- | --- |
| **AVCC** | 0–5 V | Capteurs externes type potentiomètre, tension large |
| **Référence interne 1,1 V** | 0–1,1 V | Petites tensions, capteur de température interne |

Exemple : Capteur LM35 → 10 mV/°C → 0,25 V → 25°C

- AVCC = 5V → ADC ≈ 51 (faible résolution)
- Interne 1,1V → ADC ≈ 232 (meilleure précision)

## Chemin su signal
        
```c
Entrée analogique (ADCx)
        ↓
    Multiplexeur → (sélection du canal)
        ↓
    Sample & Hold (capture la tension)
        ↓
Comparateur ↔ DAC (comparaison successive)
        ↓
Conversion logic (10 bits)
        ↓
Registre ADC Data (ADCL + ADCH)
        ↓
CPU (résultat numérique)
```
        
## Capteurs ADC
        
| Nom | Type de capteur | Broche ADC | Grandeur mesurée | Variation |
| --- | --- | --- | --- | --- |
| RV1 | Potentiomètre | ADC0 | Tension manuelle | dépend du réglage |
| R14 | LDR | ADC1 | Luminosité | + lumière → + tension |
| R20 | NTC | ADC2 | Température | + chaleur → - tension |
- LDR (Light Dependent Resistor)
- NTC (Negative Temperature Coefficient)

## Capteur interne ATmega328P
        
| Capteur | Canal | Référence | Remarque |
| --- | --- | --- | --- |
| Température MCU | ADC8 interne | 1,1 V | Pas très précis, lecture toutes les 20 ms, besoin de formule calibrée : `T = (ADC - TOS)/k` |
- Formule de calibration :

$$
T(°C)=ADC−TOSkT(°C) = \frac{\text{ADC} - TOS}{k}
$$

$$
T(°C)=kADC−TOS
$$

- `TOS` = offset EEPROM / datasheet
- `k` = coefficient datasheet

## Notes pratiques

- `ADCSRA |= (1 << ADEN)` → activer ADC
- `ADMUX` → sélection canal + référence
- `ADLAR` = 1 → lecture 8 bits directement dans ADCH
- Conversion **non bloquante** : lire quand `ADSC` = 0, relancer conversion

## Resume

| Terme                  | Signification                          | Rôle                               |
| ---------------------- | -------------------------------------- | ---------------------------------- |
| **ADC**                | Analog-to-Digital Converter            | Convertit une tension en nombre    |
| **Multiplexeur (MUX)** | Sélectionne une entrée parmi plusieurs | Choisir le canal ADC à lire        |
| **AVCC**               | Alimentation de la partie analogique   | Fournit une tension stable à l’ADC |
| **VCC / VDD**          | Alimentation principale du circuit     | Alimente la logique numérique      |
