# Guide du Makefile pour Embarque (AVR/Arduino)

Ce guide explique le fonctionnement du Makefile utilise pour programmer des microcontroleurs AVR (comme l'ATmega328p sur Arduino Uno).

## Structure du Makefile

### 1. Les Outils (Toolchain AVR)

```makefile
CC          = avr-gcc       # Compilateur C pour AVR
OBJCOPY     = avr-objcopy   # Convertisseur de formats
AVRDUDE     = avrdude       # Outil de flashage
```

**Explication :**
- **avr-gcc** : Compilateur C special pour microcontroleurs AVR. Il transforme votre code C en code machine pour l'ATmega328p.
- **avr-objcopy** : Convertit le fichier binaire (.bin) en format Intel HEX (.hex) que le microcontroleur peut comprendre.
- **avrdude** : Programme qui envoie votre code compile vers le microcontroleur via le port serie.

---

### 2. Configuration du Microcontroleur

```makefile
MCU         = atmega328p    # Type de microcontroleur
F_CPU       ?= 16000000UL   # Frequence du processeur (16 MHz)
```

**Explication :**
- **MCU** : Specifie le modele exact du microcontroleur (ATmega328p = puce de l'Arduino Uno)
- **F_CPU** : Frequence de l'horloge du processeur
  - `16000000UL` = 16 MHz (16 millions de cycles par seconde)
  - Le `UL` signifie "Unsigned Long" en C
  - Cette valeur est cruciale pour les delais et timings precis

---

### 3. Les Flags de Compilation (CFLAGS)

```makefile
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os
```

**Explication de chaque flag :**

| Flag | Signification | Explication |
|------|---------------|-------------|
| `-mmcu=$(MCU)` | Machine MCU | Indique au compilateur pour quel microcontroleur compiler (atmega328p) |
| `-DF_CPU=$(F_CPU)` | Define F_CPU | Definit une macro preprocesseur avec la frequence CPU (utilise dans vos delais) |
| `-Os` | Optimize Size | Optimise le code pour reduire sa taille (important car la memoire est limitee sur AVR) |

**Autres flags communs que vous pourriez rencontrer :**
- `-Wall` : Active tous les avertissements (utile pour debugger)
- `-Wextra` : Active encore plus d'avertissements
- `-g` : Ajoute des informations de debug
- `-O0` : Pas d'optimisation (debug)
- `-O1`, `-O2`, `-O3` : Niveaux d'optimisation croissants pour la vitesse

---

### 4. Configuration AVRDUDE

```makefile
PROGRAMMER  = arduino       # Type de programmateur
PORT        = /dev/ttyUSB0  # Port serie
BAUD        = 115200        # Vitesse de communication
```

**Explication :**
- **PROGRAMMER** : Protocole utilise (arduino = bootloader Arduino)
- **PORT** :
  - Linux : `/dev/ttyUSB0` ou `/dev/ttyACM0`
  - Windows : `COM3`, `COM4`, etc.
  - Mac : `/dev/cu.usbserial-*`
- **BAUD** : Vitesse de communication en bits par seconde (115200 = standard Arduino)

```makefile
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(NAME).hex
```

**Decomposition des flags :**
- `-c arduino` : Utilise le protocole arduino
- `-p atmega328p` : Cible le microcontroleur atmega328p
- `-P /dev/ttyUSB0` : Utilise ce port serie
- `-b 115200` : Communication a 115200 bauds
- `-U flash:w:main.hex` : Ecrit (write) le fichier .hex dans la memoire flash

---

## Les Regles (Targets)

### `make all` (par defaut)
```makefile
all: hex flash
```
Execute `hex` puis `flash` : compile ET flashe automatiquement.

### `make hex`
```makefile
hex: $(NAME).c
    $(CC) $(CFLAGS) -o $(NAME).bin $(NAME).c
    $(OBJCOPY) -O ihex -R .eeprom $(NAME).bin $(NAME).hex
```

**Etapes :**
1. Compile `main.c` en `main.bin` (binaire ELF)
2. Convertit `main.bin` en `main.hex` (format Intel HEX)
   - `-O ihex` : Format de sortie Intel HEX
   - `-R .eeprom` : Retire la section EEPROM

### `make flash`
```makefile
flash:
    $(AVRDUDE) $(AVRDUDE_FLAGS)
```
Televerse le fichier .hex vers le microcontroleur via avrdude.

### `make clean`
```makefile
clean:
    rm -f $(NAME).bin $(NAME).hex
```
Supprime les fichiers generes (.bin et .hex).

### `make re`
```makefile
re: clean all
```
Nettoie tout puis recompile et flashe (rebuild complet).

---

## Workflow Typique

```bash
# Developper du code dans main.c, puis :

make          # Compile et flashe automatiquement
# OU
make hex      # Compile seulement
make flash    # Flashe seulement

make clean    # Nettoie les fichiers generes
make re       # Rebuild complet
```

---

## Formats de Fichiers

### .c (Source)
Votre code C original (lisible par humain)

### .bin (ELF Binary)
Fichier binaire au format ELF contenant le code machine + metadata
- Non utilisable directement par le microcontroleur
- Contient symboles de debug, sections, etc.

### .hex (Intel HEX)
Format texte hexadecimal comprenant :
- Adresses memoire
- Donnees en hexadecimal
- Checksums pour verification
- **C'est ce format que le bootloader Arduino peut lire**

---

## Troubleshooting Courant

### Erreur : "Permission denied" sur /dev/ttyUSB0
```bash
sudo usermod -a -G dialout $USER
# Puis se deconnecter/reconnecter
```

### Port non trouve
Verifier le port reel :
```bash
ls /dev/tty*       # Linux
ls /dev/cu.*       # Mac
# Puis modifier PORT dans le Makefile
```

### Board non detectee
- Verifier que le cable USB est bien connecte
- Verifier que le bon port est selectionne
- Essayer de reinitialiser la board

### Erreur de compilation
- Verifier que `avr-gcc` est installe :
  ```bash
  sudo apt install gcc-avr avr-libc avrdude  # Linux
  brew install avr-gcc avrdude               # Mac
  ```

---

## Ressources Utiles

- **ATmega328p Datasheet** : Documentation complete du microcontroleur
- **AVR Libc Manual** : Documentation des fonctions C pour AVR
- **AVRDUDE Manual** : `man avrdude` pour toutes les options

---

## Resume Rapide

| Commande | Action |
|----------|--------|
| `make` ou `make all` | Compile + Flashe |
| `make hex` | Compile uniquement |
| `make flash` | Flashe uniquement |
| `make clean` | Supprime fichiers generes |
| `make re` | Rebuild complet |
