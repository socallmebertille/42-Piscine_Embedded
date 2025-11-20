# SPI (Serial Peripheral Interface)

C’est un protocole **full-duplex** à 4 fils :

| Ligne | Nom | Rôle |
| --- | --- | --- |
| **MOSI** | Master Out Slave In | données envoyées du Master vers Slave |
| **MISO** | Master In Slave Out | données envoyées du Slave vers Master |
| **SCK** | Serial Clock | horloge envoyée par le Master |
| **SS** | Slave Select | active un périphérique (niveau bas = actif) |

