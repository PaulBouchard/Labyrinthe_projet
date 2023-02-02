# Labyrinthe_projet

Le fichier fonclab.c contient les différentes fonctions et le fichier deblab.c contient le main utilisant lesdites fonctions.

La foonction coup_auto exécute un coup automatiquement en vérifiant toutes les configurations d'insertions possible puis cherche un chemin pour aller au trésor via 
la fonction expansion (cf ancien TP labyrinthe).

Si une certaine config permet au joueur d'aller  sur le trésor alors le mouvement est modifié selon cette dernière et la fonction se termine.
Dans le cas où il n'y a pas de chemin possible, le mouvement va être modifié de sorte à emmener le joueur le plus loin possible. Si cela fait deux tours que le joueur
est "bloqué" à la même position, on change la méthode utilisée lorsqu'il n'y a pas de chemin menant au trésor : le joueur sera amené au plus proche possible de la case
contenant le trésor.


J'ai cherché à optimiser cette méthode via l'algorithme minimax mais je me suis retrouvé bloqué à certaines étapes.
