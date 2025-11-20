# L’EEPROM (Electronically Erasable Programmable Read-Only Memory)

L’EEPROM est **séparée** du Flash (programme) et de la SRAM (variables).

L'EEPROM sur les AVR est une mémoire non-volatile accessible octet par octet. Points utiles pour l'ex00 :

- **Lecture** : on lit un octet avec `eeprom_read_byte((uint8_t*)addr)` (fourni par `avr/eeprom.h`).
- **Taille** : macro `E2END` (dans `avr/eeprom.h`) donne l'adresse **du dernier octet** de l'EEPROM ; la taille réelle est donc `E2END + 1`. Vérifie `#ifdef E2END` au cas où la cible n'aurait pas EEPROM.
- **Écriture** : limiter les écritures (cycles de vie ~100k). Pour cet ex00 tu ne dois **lire** que.
- **Format demandé** (d'après ton exemple) :
    - Une ligne = 16 octets
    - Affiche l'offset (en octal-like 7 chiffres dans l'exemple — on utilisera hexadécimal à 7 positions ou bien afficher exactement comme l'exemple) puis 8 groupes de 4 hex (chaque groupe = 2 octets = mot), espace entre les groupes.
    - Exemple de logique : afficher `0000000`  puis pour i=0..7 afficher `hhhh`  où `hh` est l'octet haut suivi de l'octet bas (deux octets par groupe), et la ligne suivante avance de 0x10.