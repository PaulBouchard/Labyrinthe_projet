#ifndef __FONCLAB_H__
#define __FONCLAB_H__
#include "clientAPI.h"
#include "labyrinthAPI.h"

/* ----------------------------------
 * La position d'un joueur contient : 
 * - les coordonnées x et y du joueur
 * - l'indice du trésor qu'il doit trouver
 */
typedef struct{
    int x,y;
    int nextI;
}t_position;

/* ---------------------------
 * Une tuile est définie par :
 * - la présence de mur sur ses côtés (1 = mur, 0 = libre)
 * - la présence d'un trésoren son centre (0 sil n'y en a pas, sinon l'indice du trésor)
 */
typedef struct{
    int tileN,tileS,tileW,tileE;
    int tileI;
}t_tuile;

/* ------------------------------
 * Le labyrinthe est défini par :
 * - Les positions des deux joueurs
 * - La tuile supplémentaire
*/
typedef struct{
    t_position joueur1;
    t_position joueur2;
    t_tuile tuile_supplementaire;
}t_labyrinthe;

/* ----------------------------------------------------------------------------------
 * But: Demande les différents paramètres d'un coup au joueur et récupère ceux entrés
 * Paramètres : 
 *    - mouvement : le mouvement du joueur de type t_move
 * Retourne rien
 * Modifie les paramètres du mouvement
 */
void demande_coup_joueur(t_move * mouvement);

/* ----------------------------------------------------------------
 * But: Initialise les différentes données relatives au labyrinthe
 * Paramètres : 
 *    - donnees : données du labyrinthe qu'on va modifier dans la fonction (position des joueurs et tuile supplémentaire)
 *    - case_N,case_E,case_S,case_O,case_I : informations de la tuile supplémentaire
 *    - lab : liste contenant toutes les données relatives aux tuiles du labyrinthe
 *    - tx,ty : longueur et largeur du labyrinthe
 *    - look : tableau en 2D contenant les tuiles du labyrinthe (lab mais mieux organisé)
 *    - premierJoueur : valeur indiquant qui est le joueur qui débutera la partie
 * Retourne rien
 * Modifie donnees,look
 */
void init_type(t_labyrinthe * donnees,int case_N,int case_E,int case_S,int case_O,int case_I,int * lab,int tx,int ty,t_tuile look[ty][tx],int premierJoueur,t_position liste[500]);

/* ----------------------------------
 * But: tourne la tuile sur elle-même
 * Paramètres : 
 *    - angle : valeur indiquant combien de fois on tourne la tuile de 90 degrés
 *    - tile : tuile à tourner ou non
 * Retourne 0 pour que ça serve de condition d'arrêt total de la fonction
 * Modifie les paramètres de la tuile
 */
int rotation(int angle,t_tuile *tile);

/* ---------------------------------------------------------------------------------------------------------------
 * But: vérifier si un joueur est sur une rangée qui est supposé être modifié et changer sa position en conséquence
 * Paramètres : 
 *    - positionBouge,positionBougePas : les entiers indiquant les coordonnées du joueur
 *    - plusmoins : dépendant du mouvement de la rangée, on additionne à une coordonnée du joueur pour la modifier si il
 *      est dessus
 *    - max,min : position maximal et minimal où peut potentiellement être le joueur
 * Retourne la coordonnée qui est potentiellement différente 
 * Modifie rien 
 */
int deplacementJoueur(int positionbouge,int positionbougepas,int rangee,int plusmoins,int max,int min);

/* ----------------------------------------------------------------------
 * But: met à jour les données du labyrinthe et le tableau le représentant
 * Paramètres : 
 *    - mouvement : variable de type t_move contenant les paramètres du dernier mouvement joué
 *    - donnees : données du labyrinthe
 *    - tx,ty : longueur et largeur du labyrinthe
 *    - laby : tableau de tuiles 
 *    - num_joueur : entier indiquant quel joueur vient de jouer
 * Retourne rien
 * Modifie les positions, la tuile supplémentaire, le tableau
 */
void MaJDonnees(t_move mouvement,t_labyrinthe * donnees,int tx,int ty,t_tuile laby[ty][tx],int num_joueur,t_position liste[500],int indicemouv);

/* --------------------------------------------------------------------------------------------------------------------
 * But: copier le labyrinthe dans un autre tableau et y mettre les trésors à zéro et récupérer les coordonnées du trésor
 * Paramètres : 
 *    - tx,ty : longueur et largeur du labyrinthe
 *    - labareset : tableau vide qui va copier exempleLab et avoir ses indices de trésor mis à zéro
 *    - exempleLab : tableau contenant toutes les informations sur les tuiles
 *    - arrivee : liste contenant les coordonnées du prochain trésor à trouver
 *    - tresor : indice du prochain trésor à trouver
 * Retourne rien
 * Modifie labareset et arrivee 
 */
void resetLabyrinth(int tx,int ty,t_tuile labareset[ty][tx],t_tuile exempleLab[ty][tx],int arrivee[2],int tresor);

/* ------------------------------------------------
 * But: chercher un chemin pour atteindre le trésor
 * Paramètres : 
 *    - tx,ty : longueur et largeur du labyrinthe
 *    - laby : tableau à parcourir
 *    - arrivee : liste contenant les coordonnées du prochain trésor à trouver
 *    - depart : coordonnées de départ
 * Retourne -1 si il existe un chemin pour atteindre le trésor, sinon il renvoie le nombre de pas maximum faits
 * Modifie arrivee dans le cas où le trésor ne peut être atteint
 */
int expansion(int tx,int ty,t_tuile laby[ty][tx],int depart[2],int arrivee[2],int bloque);

/* --------------------------------------------------------------------------------------------------------------
 * But: fait un coup automatiquement selon les possibles chemin que peut faire le joueur pour atteindre le trésor
 * Paramètres : 
 *    - mouvement : mouvement du joueur a modifié
 *    - donnees : positions et tuile supplémentaire servant pour voir les différentes possibilités d'insertions de tuile
 *    - tx,ty : longueur et largeur du labyrinthe
 *    - laby : tableau à parcourir et modifier selon les cas
 *    - ancienmouv : ancien mouvement pour ne pas faire l'insertion inverse
 * Retourne 1 si un chemin vers le trésor est possible, 0 sinon (sert à arrêter la fonction)
 * Modifie le mouvement
 */
int coup_auto(t_move * mouvement,t_labyrinthe donnees,int tx,int ty,t_tuile laby[ty][tx],t_move ancienmouv,t_position liste_pos[500],int indicemouv);

#endif