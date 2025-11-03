<div align="center" class="text-center">
  <h1>42-Piscine_Embedded</h1>
  
  <img alt="last-commit" src="https://img.shields.io/github/last-commit/socallmebertille/42-Piscine_Embedded?style=flat&amp;logo=git&amp;logoColor=white&amp;color=0080ff" class="inline-block mx-1" style="margin: 0px 2px;">
  <img alt="repo-top-language" src="https://img.shields.io/github/languages/top/socallmebertille/42-Piscine_Embedded?style=flat&amp;color=0080ff" class="inline-block mx-1" style="margin: 0px 2px;">
  <img alt="repo-language-count" src="https://img.shields.io/github/languages/count/socallmebertille/42-Piscine_Embedded?style=flat&amp;color=0080ff" class="inline-block mx-1" style="margin: 0px 2px;">
  <p><em>Built with the tools and technologies:</em></p>
  <img alt="Markdown" src="https://img.shields.io/badge/Markdown-000000.svg?style=flat&amp;logo=Markdown&amp;logoColor=white" class="inline-block mx-1" style="margin: 0px 2px;">
  <img alt="GNU%20Bash" src="https://img.shields.io/badge/GNU%20Bash-4EAA25.svg?style=flat&amp;logo=GNU-Bash&amp;logoColor=white" class="inline-block mx-1" style="margin: 0px 2px;">
  <img alt="C" src="https://img.shields.io/badge/Language-2496ED.svg?style=flat&amp;logo=c&amp;logoColor=white" class="inline-block mx-1" style="margin: 0px 2px;">
</div>

<h2>Table of Contents</h2>
<ul class="list-disc pl-4 my-0">
  <li class="my-0"><a href="#overview">Overview</a></li>
  <ul class="list-disc pl-4 my-0">
    <li class="my-0"><a href="#initialization-on-windows-avec-terminal-wsl">Initialization on Windows avec terminal WSL</a></li>
    <li class="my-0"><a href="#few-embedded-notions">Few Embedded notions</a></li>
  </ul>
  <li class="my-0"><a href="#ok">Ok</a>
  <ul class="list-disc pl-4 my-0">
    <li class="my-0"><a href="#ok">Ok</a></li>
  </ul>
  </li>
</ul>

<h2>Overview</h2>
<h3>Initialization on Windows avec terminal WSL</h3>

Ouvrir Windows Powershel en tant qu'administrateur, puis :

| Étape | Commande                                      | Objectif                                     |
| ----- | --------------------------------------------- | -------------------------------------------- |
| 1     | `usbipd list`                                 | List les ports et trouve le microcontrolleur |
| 2     | `usbipd detach --busid x-x`                   | Libère le port x-x                           |
| 3     | `usbipd attach --wsl --busid x-x`             | Attache au bon WSL                           |

Et finalement sur le terminal WSL :
| Étape | Commande                                      | Objectif                                     |
| ----- | --------------------------------------------- | -------------------------------------------- |
| 4     | `ls /dev/ttyUSB*`                             | Quelque chose comme `/dev/ttyUSB0`           |
| 5     | `make flash` (ou `avrdude ...`)               | Flashe le code                               |

<h3>Few Embedded notions</h3>

#### Les concepts

Un microcontrôleur doit parfois échanger des données avec un autre appareil :
- ton PC (via USB),
- un autre microcontrôleur,
- un capteur ou module externe (GPS, Bluetooth, etc.).
Il existe deux grands types de communication :

| Type	    | Exemple	Description                                                                |
|-----------|------------------------------------------------------------------------------------|
| Parallèle	| ancien bus IDE, vieux écrans plusieurs fils → chaque bit passe sur un fil séparé   |
| Série   	| USB, UART, SPI, I2C…	1 seul fil pour envoyer les bits un à un, l’un après l’autre |

#### Les outils

- `avr-gcc`	→ Compile ton code C pour l’architecture AVR
- `avr-objcopy`	→ Convertit le binaire en format `.hex`
- `avrdude` →	Transfère le programme sur la carte via USB

#### Les ports (= groupes de broche)

- Port `B` = broches numériques 8 à 13 (souvent reliées à LEDs, SPI, etc.)
- Port `C` = broches analogiques (A0–A5)
- Port `D` = broches 0–7 (souvent RX/TX, boutons, etc.)
Chacun de ces ports est contrôlé par 3 registres.

#### Les registres

Un registre est une petite zone mémoire directement à l’intérieur du microcontrôleur, souvent de 8 bits.
Chaque bit représente un interrupteur matériel 🕹️, qui contrôle une fonction très précise du processeur ou d’un périphérique (GPIO, Timer, UART, etc.).
En `C`, les registres sont déclarés sur la librairie `<avr/io.h>`.

| Nom       | Signification           | Exemple                                                      |
| --------- | ----------------------- | ------------------------------------------------------------ |
| **DDRx**  | Data Direction Register | configure la direction (entrée ou sortie)                    |
| **PORTx** | Output Register         | crit un état logique/valeur (HIGH ou LOW)                    |
| **PINx**  | Input Register          | lit l’état logique/valeur d’une broche (si c'est une entrée) |

#### Les bits

1 octet = 8 bits

Exemple : `0b10101100` (le `0b` indique du binaire en `C`).
Chaque bit correspond à une puissance de 2 : 
```
bit:   7  6  5  4 3 2 1 0
val: 128 64 32 16 8 4 2 1
```

| Opérateur | Nom               | Exemple                      | Résultat                         |
| --------- | ----------------- | ---------------------------- | -------------------------------- |
| `&`       | ET logique        | `a & b`                      | 1 si les deux bits sont 1        |
| `\|`      | OU logique        | `a \| b`                     | 1 si au moins un bit est 1       |
| `^`       | XOR (OU exclusif) | `a ^ b`                      | 1 si un seul des deux bits est 1 |
| `~`       | NON logique       | `~a`                         | inverse tous les bits            |
| `<<`      | Décalage à gauche | `1 << 3` → `0b00001000`      | utile pour positionner un bit    |
| `>>`      | Décalage à droite | `0b1000 >> 3` → `0b00000001` | inverse du précédent             |

Exemple : 
- `PORTB |= (1 << PB0);` → met à 1 le bit 0 sans toucher aux autres bits.
- `PORTB &= ~(1 << PB0);` → met à 0 le bit 0 sans affecter les autres.
- `if (PINB & (1 << PB1)) { // le bouton relié à PB1 est pressé }` → l'input de la broche B 
- `(1 << 0) = 0001` →  est un `mask`, `1 << 0` veut dire : décale le 1 de zéro position → donc
`(1 << 0) = 0b00000001` (en binaire) = `1` (en décimal).

#### Le pull-up

Une **résistance** qui "tire vers le haut" (VCC) le signal quand rien n'est connecté.

##### Sans pull-up : ❌ PROBLÈME

```
        VCC (+5V)
           │
         [SW1]
           │
           ├──────→ PD2 (lecture)
           │
          ??? ← Pin FLOTTANTE (lecture aléatoire)
```

##### Avec pull-up : ✅ STABLE

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

##### Activation dans le code

```c
DDRD &= ~(1 << PD2);    // 1. Configurer en ENTRÉE
PORTD |= (1 << PD2);    // 2. Activer pull-up interne
```

##### États

| Bouton | Circuit | PD2 |
|--------|---------|-----|
| **Relâché** | VCC → Pull-up → PD2 | **HIGH** (5V) |
| **Appuyé** | VCC → SW1 → GND | **LOW** (0V) |

##### Logique de lecture

```c
if (!(PIND & (1 << PD2)))  // Si LOW → bouton appuyé
```

#### L'anti-rebond (= debouncing effect)

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

##### Technique de debouncing

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

#### `screen` : terminal série en ligne de commande

`screen` est un terminal virtuel qu’on peut attacher directement à un périphérique série (`/dev/ttyUSB0`, `/dev/ttyACM0`, etc.).
Il sert à écouter et envoyer des données sur le port série de ton microcontrôleur.

| Action                | Commande                                      |
| --------------------- | --------------------------------------------- |
| Lancer `screen`       | `screen /dev/ttyUSB0 115200`                  |
| Quitter proprement    | `Ctrl + A`, puis `Ctrl + D` ou taper `:quit`  |
| Détacher              | `Ctrl + A`, `D`                               |
| Lister les sessions   | `screen -ls`                                  |
| Rejoindre une session | `screen -r`                                   |
| Forcer la fermeture   | `sudo fuser -k /dev/ttyUSB0`                  |



<h2>Ok</h2>
<h3>Ok</h3>