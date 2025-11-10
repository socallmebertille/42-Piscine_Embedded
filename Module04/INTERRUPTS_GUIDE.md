# Les interrupts & ISR (Interrupt Service Routines)
    
Une **interruption** est un signal matériel ou logiciel qui **interrompt le flux normal du programme** pour exécuter un code spécifique (appelé *routine d’interruption*).
    
## Utilisation
        
➡️ Utilisé pour gérer des événements asynchrones :

- Timer arrivé à zéro
- Réception série terminée
- Bouton appuyé
- Watchdog, etc.

## Fonctionnement général
1. **Le programme principal** s’exécute normalement.
2. Un **événement matériel** ou logiciel survient.
3. Le microcontrôleur :
    - Sauvegarde le contexte CPU (PC, registres, flags)
    - **Saute à une adresse fixe** contenue dans la *table des vecteurs d’interruption*
4. Exécute la fonction associée à cette interruption → **l’ISR**
5. À la fin, exécute l’instruction spéciale `RETI` (retour d’interruption) qui :
    - Restaure le contexte
    - Rétablit les interruptions globales

## La table des vecteurs
        
Au démarrage, le MCU a une **table de vecteurs** à une adresse fixe (souvent en Flash, au tout début de la mémoire).

Chaque entrée contient **l’adresse d’une ISR** :

| Vecteur | Nom du signal | Fonction associée |
| --- | --- | --- |
| 0 | Reset | `__vector_0` |
| 1 | External Interrupt 0 | `__vector_1` |
| 2 | External Interrupt 1 | `__vector_2` |
| … | … | … |
| 16 | Timer0 Overflow | `__vector_16` |
| … | etc. | … |

👉 Ces adresses sont **fixées à la compilation**, pas modifiables à l’exécution.
        
## Déclaration d’une ISR en C (AVR)
        
Dans les toolchains AVR (ex: avr-gcc), on utilise la macro :

```c
#define ISR(n) __attribute__((signal)) void __vector_##n()
```

Elle permet d’écrire :

```c
ISR(16)
{
    // Code à exécuter quand le Timer0 overflow se produit
}
```

ou, avec des alias lisibles issus de “isr.h” :

```c
#define ISR_TC0_OVERFLOW ISR(16)

ISR_TC0_OVERFLOW
{
    // Ton code ici
}
```

Le mot-clé spécial `__attribute__((signal))` :

- indique que la fonction est une ISR (pas une fonction normale),
- génère une instruction `RETI` à la fin (et non `RET`).

## Activation des interruptions
        
Pour qu’une interruption fonctionne :

1. Configurer le périphérique (ex: activer l’interruption du timer ou du UART)
2. Autoriser les interruptions globalement :

```c
cli();  // Clear (désactive) les interruptions
sei();  // Set Enable Interrupts
```

(`sei()` et `cli()` sont des instructions assembleur wrappers)
        
## Résumé visuel

```c
┌──────────────┐
│ Programme C  │
│   principal  │
└──────┬───────┘
       │
       ▼
   ┌──────────────┐
   │ Événement ?  │─── Non ──> Boucle principale
   └──────┬───────┘
          │Oui
          ▼
┌──────────────────────────────┐
│ MCU saute vers vecteur n     │
│ Exécute ISR correspondante   │
│ Restaure contexte (RETI)     │
└──────────────────────────────┘

```