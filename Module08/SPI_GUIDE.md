# SPI (Serial Peripheral Interface)

## Principe

C’est un protocole **full-duplex** à 4 fils :

| Ligne | Nom | Rôle |
| --- | --- | --- |
| **MOSI** | Master Out Slave In | données envoyées du Master vers Slave |
| **MISO** | Master In Slave Out | données envoyées du Slave vers Master |
| **SCK** | Serial Clock | horloge envoyée par le Master |
| **SS** | Slave Select | active un périphérique (niveau bas = actif) |

## Master VS Slave
    
Le **Master** :

- génère l’horloge `SCK`
- contrôle le débit
- choisit quel slave parler avec (via `SS`)

Le **Slave** :

- répond à l’horloge
- retourne un octet en même temps qu’il en reçoit un

## Communication full-duplex
    
Quand le master envoie un octet sur MOSI :

- simultanément, le slave renvoie un octet sur MISO

Un transfert SPI = **8 bits shiftés dans un registre (SPDR)**

- À chaque bit envoyé → un bit reçu
- À la fin : interruption interne → flag SPIF = 1