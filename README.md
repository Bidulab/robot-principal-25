# robot-principal-25
Programmation du robot principal de la CDFR 2025 de l'équipe Bidulab.

Vous trouverez les programmes Remote et Robot ainsi que des tests, pour être compilés pour Arduino Nano et Mega.

Programme testé avec Arduino IDE 2.2.1
### Dépendances
#### Remote
- Aucune dépendance
#### Robot
- TM1637 by Avishay Orpaz
https://github.com/avishorp/TM1637

- TMC2209 by Peter Polidoro
https://github.com/janelia-arduino/TMC2209

# Programme Remote

## Vue d'ensemble

Ce programme Arduino permet de lire les entrées de plusieurs joysticks, boutons, et d'un encodeur rotatif. Les données sont traitées puis envoyées via une communication série à un dispositif connecté. Le dispositif doit envoyer une commande pour demander l'état actuel des entrées.

Le programme gère :
- **Deux joysticks** (chacun avec des axes X et Y analogiques et un bouton-poussoir intégré).
- **Un encodeur rotatif** (avec un bouton-poussoir et des signaux DT et CLK).
- **Quatre boutons supplémentaires**.
- **Une LED** (utilisée comme indicateur d'état).

---

## Fonctionnalités
1. **Entrées des joysticks :** Capture des valeurs analogiques pour les axes X et Y et des états numériques pour les boutons-poussoirs.
2. **Encodeur rotatif :** Suivi des mouvements relatifs (delta) et de l'état du bouton.
3. **État des boutons :** Lecture de l'état des quatre boutons supplémentaires.
4. **Indicateur LED :** Clignote si aucune communication valide n'est reçue dans un délai de 200 ms ; reste allumée sinon.
5. **Protocole personnalisé :** Un protocole léger pour transmettre les états des entrées.

---

## Détails du protocole

### Communication
- **Vitesse de transmission :** 9600 bauds.
- **Format des données :** Messages série de 13 octets.

### Structure du message
Chaque message comporte 13 octets et suit ce format :

| Octet | Description                   | Valeurs possibles     |
|-------|-------------------------------|-----------------------|
| 0     | Octet de début                | `0x41` (ASCII 'A')    |
| 1     | Axe X du joystick 1 (échelle) | `0-255`               |
| 2     | Axe Y du joystick 1 (échelle) | `0-255`               |
| 3     | Axe X du joystick 2 (échelle) | `0-255`               |
| 4     | Axe Y du joystick 2 (échelle) | `0-255`               |
| 5     | État du bouton 1              | `0` ou `1`            |
| 6     | État du bouton 2              | `0` ou `1`            |
| 7     | État du bouton 3              | `0` ou `1`            |
| 8     | État du bouton 4              | `0` ou `1`            |
| 9     | État du bouton du joystick 1  | `0` ou `1`            |
| 10    | État du bouton du joystick 2  | `0` ou `1`            |
| 11    | Delta de l'encodeur rotatif   | Valeur signée         |
| 12    | Octet de fin                  | `0x55`                |

---

### Requête des données
Le programme Arduino envoie des données uniquement lorsqu’il reçoit une commande spécifique du dispositif connecté. 

- **Commande de requête :** Le dispositif doit envoyer un octet contenant la valeur `0x41` (ASCII 'A'). À la réception de cette commande, l’Arduino répond avec les états actuels des entrées dans le format décrit ci-dessus.

---

### Indicateur LED
- La LED clignote (200 ms éteinte, 150 ms allumée) si aucune commande valide (`0x41`) n'est reçue pendant plus de 200 ms.
- La LED reste allumée lorsque les messages valides sont reçus régulièrement.