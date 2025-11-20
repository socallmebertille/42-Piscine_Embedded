# L’I2C (Inter-Integrated Circuit)

- **Protocole de communication série** inventé par Philips dans les années 1980.
- Permet à **plusieurs périphériques** (capteurs, mémoires, afficheurs…) de communiquer avec un **microcontrôleur ou un ordinateur**.
- I²C = un bus de communication basé sur des signaux synchronisés
- Utilise seulement **2 fils** :
    - **SDA** → Data (données)
    - **SCL** → Clock (horloge)
- Ce sont des lignes **open-drain** → ça veut dire :
    - Les composants **ne tirent les lignes qu'à 0**
    - Et les résistances de pull-up ramènent les lignes à 1

## Notion de **bus**
    
C’est un **bus de communication série** → les données passent **bit par bit sur deux fils** :

| Fil | Nom | Rôle |
| --- | --- | --- |
| **SDA** | Serial Data | transporte les données |
| **SCL** | Serial Clock | donne le rythme (l’horloge) |

🧩 Les deux fils sont **partagés entre tous les composants**.

→ C’est pour ça qu’on parle de **bus** : un seul chemin commun pour tout le monde.

## Comment les signaux forment un protocole ?
    
L’horloge (SCL) cadence la transmission :

- Quand SCL = 1, SDA ne doit pas changer (sauf pour START/STOP)
- Les données sont valides sur fronts montants/descendants

Donc physiquement :

```c
SCL:  _|‾|_|‾|_|‾|_
SDA:  1 0 1 1 0 0 1
```

Chaque bit est lu **quand SCL est haut**.

## Les signaux START et STOP
    
START et STOP sont des **patterns de signaux**, pas des données.

### START = SDA descend quand SCL est haut

```c
SCL:  ___‾‾‾‾‾‾
SDA:  ‾‾‾\____     (SDA tombe pendant SCL = 1)
```

### STOP = SDA monte quand SCL est haut

```c
SCL:  ___‾‾‾‾‾‾
SDA:  ____/‾‾‾     (SDA monte pendant SCL = 1)
```

Donc le µC (= microcontrôleur), **dans son hardware TWI**, “observe le bus” et reconnaît :

> “Ah ! Le pattern SDA↓ quand SCL↑ → START signal.”
> 

Et il écrit le **code 0x08 dans TWSR** (TW_START).

## Pourquoi I²C fonctionne grâce à des signaux synchronisés ?
    
Parce que :

- SCL impose le rythme → tous les composants sont synchrones
- SDA est lu au rythme de SCL
- Les lignes open-drain évitent les conflits
- Les résistances pull-up stabilisent le bus
- START / STOP sont des motifs logiques reconnus par tous

I²C = un **langage de signaux** sur 2 fils.

## Schéma des signaux
    
```c
            START             ADDR + W          ACK           DATA       
SDA:   ___\__________________XXXXXXXXX_________0_____________XXXXXXXX__
SCL:   _____|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_____|‾|_|‾|_|‾|_____|‾|

            ACK            STOP
SDA:  _____0______________/‾‾‾
SCL:  _|‾|___________
```
    
## Le modèle **Master / Slave**
    
C’est juste une façon d’organiser **qui commande** et **qui obéit** sur ce bus.

| Rôle | Description | Exemple concret |
| --- | --- | --- |
| **Master** | Celui qui **initie la communication** (il parle en premier, décide à qui parler, quand, et quoi lire/écrire) | Le **microcontrôleur** (ton ATmega328P) |
| **Slave** | Celui qui **répond** quand le master s’adresse à lui (grâce à son adresse unique) | Un **capteur** de température, une **mémoire**, un **afficheur**... |

🧠 **Analogie simple :**

> Le master, c’est le professeur.
> 
> 
> Les slaves, ce sont les **élèves**.
> 
> Le prof (master) pose une question à un élève (slave) par son prénom (adresse),
> 
> l’élève répond — et personne d’autre ne parle en même temps.
> 

<aside>
💡

Communication **point-à-point** possible → le master peut parler directement à un seul slave.

</aside>

## Schema pour 2 slaves

```c
      +----------------------+
      |  Microcontrôleur     |   (Master)
      |   SDA  ──────────────┬──────────── SDA (Data)
      |   SCL  ──────────────┬──────────── SCL (Clock)
      +----------------------+
                     |                |
                +----------+     +----------+
                | Capteur  |     | Mémoire  |
                |  Temp.   |     | EEPROM   |
                | (Slave1) |     | (Slave2) |
                +----------+     +----------+
```

## Resume

| Élément | Rôle | Exemple |
| --- | --- | --- |
| **Bus I2C** | Ligne de communication commune | 2 fils (SDA/SCL) |
| **Master** | Celui qui contrôle la conversation | ATmega328P |
| **Slave** | Celui qui répond | Capteur, mémoire, afficheur |
| **SDA** | Données | Transfert bidirectionnel |
| **SCL** | Horloge | Donne le tempo des échanges |
| **ACK (Acknowledge)** | **récepteur** doit répondre **ACK** en tirant la ligne SDA à **0** durant le 9ᵉ cycle d’horloge. 👉 SDA = 0 pendant le 9ᵉ bit = **ACK** | “J’ai bien reçu” |
| NACK (Not Acknowledge) | récepteur **ne tire pas** SDA à 0 (laisse à 1 via pull-up), alors : 👉 SDA = 1 pendant le 9ᵉ bit = **NACK**. | “Je n’ai pas reçu” ou “J'ai fini” |
