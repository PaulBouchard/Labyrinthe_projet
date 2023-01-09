#include <stdlib.h>
#include <stdio.h>
#include "labyrinthAPI.h"
#include "clientAPI.h"
#include "fonclab.h"


/* ----------------------------------------------------------------------------------
 * But: Demande les différents paramètres d'un coup au joueur et récupère ceux entrés
 * Paramètres : 
 *    - mouvement : le mouvement du joueur de type t_move
 * Retourne rien
 * Modifie les paramètres du mouvement
 */
void demande_coup_joueur(t_move * mouvement){
    int intermediaire_typenum;
    printf("Entrez un chiffre entre 0 et 3 indiquant par quel côté la piece sera insérée (0=O,1=E,2=N,S=3)\n");
    scanf("%d",&intermediaire_typenum);
    mouvement->insert = intermediaire_typenum;
        
    printf("Entrez le numéro de la ligne ou de la colonne dans laquelle ira la pièce\n");
    scanf("%d",&mouvement->number);
        
    printf("Entrez un chiffre entre 0 et 3 indiquant la rotation appliquée à la pièce en sens horaire\n");
    scanf("%d",&mouvement->rotation);

    printf("Entrez les coordonnées de la case où vous voulez aller (x y)\n");
    scanf("%d %d",&mouvement->x,&mouvement->y);
}

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
void init_type(t_labyrinthe * donnees,int case_N,int case_E,int case_S,int case_O,int case_I,int * lab,int tx,int ty,t_tuile look[ty][tx],int premierJoueur){
    if (premierJoueur == 0){
        /* Initialisation de la position et du prochain trésor à trouver du joueur 1 */
        donnees->joueur1.x = 0;
        donnees->joueur1.y = 0;
        donnees->joueur1.nextI = 1;

        /* Initialisation de la position et du prochain trésor à trouver du joueur 2 */
        donnees->joueur2.x = tx - 1;
        donnees->joueur2.y = ty - 1;
        donnees->joueur2.nextI = 24;
    }
    else if (premierJoueur == 1){
        /* Initialisation de la position et du prochain trésor à trouver du joueur 1 */
        donnees->joueur1.x = tx-1;
        donnees->joueur1.y = ty-1;
        donnees->joueur1.nextI = 24;

        /* Initialisation de la position et du prochain trésor à trouver du joueur 2 */
        donnees->joueur2.x = 0;
        donnees->joueur2.y = 0;
        donnees->joueur2.nextI = 1;
    }

    /* Initialisation de la tuile supplémentaire */
    donnees->tuile_supplementaire.tileN = case_N;
    donnees->tuile_supplementaire.tileE = case_E;
    donnees->tuile_supplementaire.tileS = case_S;
    donnees->tuile_supplementaire.tileW = case_O;
    donnees->tuile_supplementaire.tileI = case_I;

    /* Remplissage du tableau labyrinthe via une liste intermédiaire */
    t_tuile * labyrinthe_inter = malloc(5*tx*ty*sizeof(int));
    int j = 0;
    for (int i = 0;i < 5*tx*ty;i = i+5){
        labyrinthe_inter[j].tileN = lab[i];
        labyrinthe_inter[j].tileE = lab[i+1];
        labyrinthe_inter[j].tileS = lab[i+2];
        labyrinthe_inter[j].tileW = lab[i+3];
        labyrinthe_inter[j].tileI = lab[i+4];
        j = j + 1;
    }

    int n=0;
    int m=0;
    for (int k = 0;k<tx*ty;k++){
        if (n==tx){
            n=0;
            m=m+1;
        }
        look[m][n] = labyrinthe_inter[k];
        n=n+1;
    }
}

/* ----------------------------------
 * But: tourne la tuile sur elle-même
 * Paramètres : 
 *    - angle : valeur indiquant combien de fois on tourne la tuile de 90 degrés
 *    - tile : tuile à tourner ou non
 * Retourne 0 pour que ça serve de condition d'arrêt total de la fonction
 * Modifie les paramètres de la tuile
 */
int rotation(int angle,t_tuile *tile){
    int inter = tile->tileN;
    
    /* Condition d'arrêt */
    if (angle == 0){
        return 0;
    }
    
    /* changement des valeurs pour une rotation de 90 degrés */
    else{
        tile->tileN = tile->tileW;
        tile->tileW = tile->tileS;
        tile->tileS = tile->tileE;
        tile->tileE = inter;
        rotation(angle-1,tile); // Appel récursif pour si la rotation est >= 90 degrés
    }
    return 0;   // Au cas où
}

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
int deplacementJoueur(int positionbouge,int positionbougepas,int rangee,int plusmoins,int max,int min){
    if (positionbougepas == rangee){
        if (positionbouge == max){
            return min;
        }
        else{
            return (positionbouge + plusmoins);
        }
    }
    return positionbouge;
}

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
void MaJDonnees(t_move mouvement,t_labyrinthe * donnees,int tx,int ty,t_tuile laby[ty][tx],int num_joueur){
    t_tuile inter;

    /* Modification du labyrinthe selon la tuile insérée et le côté choisi et modif de la position des joueurs*/
    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des lignes vers la droite */
    if (mouvement.insert == 0){
        /* Mise à jour d'une ligne du labyrinthe */
        inter = laby[mouvement.number][tx-1];
        for (int i = tx-1;i > 0;i--){
            laby[mouvement.number][i] = laby[mouvement.number][i-1];
        }

        /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la ligne */
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        laby[mouvement.number][0] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;

        /* Mise à jour des position des joueurs au cas où ils soient sur la ligne modifiée */
        donnees->joueur1.x = deplacementJoueur(donnees->joueur1.x,donnees->joueur1.y,mouvement.number,1,tx-1,0);
        donnees->joueur2.x = deplacementJoueur(donnees->joueur2.x,donnees->joueur2.y,mouvement.number,1,tx-1,0);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des lignes vers la gauche */
    else if (mouvement.insert == 1){
        /* Mise à jour d'une ligne du labyrinthe */
        inter = laby[mouvement.number][0];
        for (int i = 0;i < tx-1;i++){
            laby[mouvement.number][i] = laby[mouvement.number][i+1];
        }

        /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la ligne */
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        laby[mouvement.number][tx-1] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;

        /* Mise à jour des position des joueurs au cas où ils soient sur la ligne modifiée */
        donnees->joueur1.x = deplacementJoueur(donnees->joueur1.x,donnees->joueur1.y,mouvement.number,-1,0,tx-1);
        donnees->joueur2.x = deplacementJoueur(donnees->joueur2.x,donnees->joueur2.y,mouvement.number,-1,0,tx-1);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des colonnes vers le bas */
    else if (mouvement.insert == 2){
        /* Mise à jour d'une colonne du labyrinthe */
        inter = laby[ty-1][mouvement.number];
        for (int i = ty-1;i>0;i--){
            laby[i][mouvement.number] = laby[i-1][mouvement.number];
        }

        /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la colonne */
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        laby[0][mouvement.number] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;

        /* Mise à jour des position des joueurs au cas où ils soient sur la colonne modifiée */
        donnees->joueur1.y = deplacementJoueur(donnees->joueur1.y,donnees->joueur1.x,mouvement.number,1,ty-1,0);
        donnees->joueur2.y = deplacementJoueur(donnees->joueur2.y,donnees->joueur2.x,mouvement.number,1,ty-1,0);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des colonnes vers le haut */
    else if (mouvement.insert == 3){
        /* Mise à jour d'une colonne du labyrinthe */
        inter = laby[0][mouvement.number];
        for (int i = 0;i < ty-1;i++){
            laby[i][mouvement.number] = laby[i+1][mouvement.number];
        }

        /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la colonne */
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        laby[ty-1][mouvement.number] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;

        /* Mise à jour des position des joueurs au cas où ils soient sur la colonne modifiée */
        donnees->joueur1.y = deplacementJoueur(donnees->joueur1.y,donnees->joueur1.x,mouvement.number,-1,0,ty-1);
        donnees->joueur2.y = deplacementJoueur(donnees->joueur2.y,donnees->joueur2.x,mouvement.number,-1,0,ty-1);
    }
    
    /* Récupération de la position et du prochain trésor du joueur 2 */
    if (num_joueur == 1){
        donnees->joueur2.x = mouvement.x;
        donnees->joueur2.y = mouvement.y;
        donnees->joueur2.nextI = mouvement.nextItem;
    }

    /* Récupération de la position et du prochain trésor du joueur 1 */
    if (num_joueur == 0){
        donnees->joueur1.x = mouvement.x;
        donnees->joueur1.y = mouvement.y;
        donnees->joueur1.nextI = mouvement.nextItem;
    }
}

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
void resetLabyrinth(int tx,int ty,t_tuile labareset[ty][tx],t_tuile exempleLab[ty][tx],int arrivee[2],int tresor){
    for (int i = 0;i < ty;i++){
        for (int j = 0;j < tx;j++){
            labareset[i][j] = exempleLab[i][j];
            if (tresor == exempleLab[i][j].tileI){
                arrivee[0] = i;
                arrivee[1] = j;
            }
            labareset[i][j].tileI = 0;
        }
    }
}

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
int expansion(int tx,int ty,t_tuile laby[ty][tx],int depart[2],int arrivee[2]){
    int r = 1;
    int indice = 1;
    int max = 0;
    laby[depart[0]][depart[1]].tileI = r;
    int parcours_case = 0;
    laby[arrivee[0]][arrivee[1]].tileI = 0;
    
    while (laby[arrivee[0]][arrivee[1]].tileI == 0){
        for (int i = 0;i < ty; i++){
            for (int j = 0;j < tx; j++){
                if ((i == depart[0])&&(j == depart[1])){
                    continue;
                }
                else{
                    if (laby[i][j].tileI <= 0){
                        if ((laby[i+1][j].tileI == r) && (i<ty-1) && (i>=0) && (laby[i][j].tileS == 0) && (laby[i+1][j].tileN == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i-1][j].tileI == r) && (i>0) && (i<=ty-1) && (laby[i][j].tileN == 0) && (laby[i-1][j].tileS == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i][j+1].tileI == r) && (j<tx-1) && (j>=0) && (laby[i][j].tileE == 0) && (laby[i][j+1].tileW == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i][j-1].tileI == r) && (j>0) && (j<=tx-1) && (laby[i][j].tileW == 0) && (laby[i][j-1].tileE == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                    }
                }
            }
        }
        if (parcours_case == 0){
            
            while (max == 0){
                for (int i = -indice;i <= indice;i++){
                    if ((arrivee[0] == 0) && (i < 0)){
                        continue;
                    }
                    if (arrivee[0]+i < 0){
                        continue;
                    }
                    if (arrivee[0]+i > ty-1){
                        continue;
                    }
                    if ((arrivee[0] == ty-1) && (i > 0)){
                        continue;
                    }
                    for (int j = -indice;j <= indice;j++){
                        if ((arrivee[1] == 0) && (j < 0)){
                            continue;
                        }
                        if (arrivee[1]+j < 0){
                            continue;
                        }
                        if (arrivee[1]+j > tx-1){
                            continue;
                        }
                        if ((arrivee[1] == tx-1) && (j > 0)){
                            continue;
                        }
                        if ((i == 0) && (j == 0)){
                            continue;
                        }
                        if (laby[arrivee[0]+i][arrivee[1]+j].tileI != 0){
                            max = laby[arrivee[0]+i][arrivee[1]+j].tileI;
                            arrivee[0] = arrivee[0]+i;
                            arrivee[1] = arrivee[1]+j;
                            return 1;
                        }
                    }
                }
                indice = indice + 1;
            }

            for (int i = 0;i < ty;i++){
                for (int j = 0;j < tx;j++){
                    if (laby[i][j].tileI == r){
                        arrivee[0] = i;
                        arrivee[1] = j;
                    }
                }
            }
            return r;
        }
        else{
            parcours_case = 0;
            r = r + 1;
        }
    }
    return -1;
}

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
int coup_auto(t_move * mouvement,t_labyrinthe donnees,int tx,int ty,t_tuile laby[ty][tx],t_move ancienmouv){
    /* Déclaration des variables (intermédiaire pour la plupart) */
    t_tuile tuile_suppinter = donnees.tuile_supplementaire;
    t_tuile inter;
    t_tuile labinter[ty][tx];
    int depart[2] = {donnees.joueur1.y,donnees.joueur1.x};
    int arrivee[2];
    int chemin;


    /* insert = 0 */
    for (int i = 1;i < ty;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((ancienmouv.insert == 1) && (ancienmouv.number == i)){
            continue;
        }

        for (int k = 0;k < 4;k++){
            /* On intialise le labyrinthe avec toutes les cases item à 0 et on collecte les coordonnées d'arrivée */
            resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);

            /* Si le trésor est à l'extrémité et peut être poussée hors du labyrinthe, on skip ce coup */
            if ((arrivee[0] == i) && (arrivee[1] == tx-1)){
                continue;
            }
            tuile_suppinter = donnees.tuile_supplementaire;
            tuile_suppinter.tileI = 0;
            depart[0] = donnees.joueur1.y;
            depart[1] = donnees.joueur1.x;

            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[i][tx-1];
            for (int m = tx-1;m > 0;m--){
                labinter[i][m] = labinter[i][m-1];
            }
            rotation(k,&tuile_suppinter);
            labinter[i][0] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur et du trésor dans le cas où ils soient sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[1] = deplacementJoueur(donnees.joueur1.x,donnees.joueur1.y,i,1,tx-1,0);
            arrivee[1] = deplacementJoueur(arrivee[1],arrivee[0],i,1,tx-1,0);

            /* Si le trésor est dans la tuile supplémentaire, on prend pour coord d'arrivée celle de la tuile insérée */
            if (donnees.joueur1.nextI == donnees.tuile_supplementaire.tileI){
                arrivee[0] = i;
                arrivee[1] = 0;
            }

            /* On vérifie si un chemin existe */
            chemin = expansion(tx,ty,labinter,depart,arrivee);

            /* Si un chemin existe on modifie le mouvement accordément puis on termine la fonction */
            if (chemin == -1){
                mouvement->insert = 0;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
                return 1;
            }
            /* Sinon on réinitialise l'arrangement de labinter ainsi que la tuile supplémentaire et la position du joueur */
            else{
                mouvement->insert = 0;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
            }
        }
    }
    
    /* insert = 1 */
    for (int i = 1;i < ty;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((ancienmouv.insert == 0) && (ancienmouv.number == i)){
            continue;
        }

        for (int k = 0;k < 4;k++){
            /* On intialise le labyrinthe avec toutes les cases item à 0 et on collecte les coordonnées d'arrivée */
            resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);

            /* Si le trésor est à l'extrémité et peut être poussée hors du labyrinthe, on skip ce coup */
            if ((arrivee[0] == i) && (arrivee[1] == 0)){
                continue;
            }
            tuile_suppinter = donnees.tuile_supplementaire;
            tuile_suppinter.tileI = 0;
            depart[0] = donnees.joueur1.y;
            depart[1] = donnees.joueur1.x;
        
            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[i][0];
            for (int m = 0;m < tx-1;m++){
                labinter[i][m] = labinter[i][m+1];
            }
            rotation(k,&tuile_suppinter);
            labinter[i][tx-1] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur et du trésor dans le cas où ils soient sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[1] = deplacementJoueur(donnees.joueur1.x,donnees.joueur1.y,i,-1,0,tx-1);
            arrivee[1] = deplacementJoueur(arrivee[1],arrivee[0],i,-1,0,tx-1);

            /* Si le trésor est dans la tuile supplémentaire, on prend pour coord d'arrivée celle de la tuile insérée */
            if (donnees.joueur1.nextI == donnees.tuile_supplementaire.tileI){
                arrivee[0] = i;
                arrivee[1] = tx-1;
            }

            /* On vérifie si un chemin existe */
            chemin = expansion(tx,ty,labinter,depart,arrivee);

            /* Si un chemin existe on modifie le mouvement accordément puis on termine la fonction */
            if (chemin == -1){
                mouvement->insert = 1;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
                return 1;
            }
            /* Sinon on réinitialise l'arrangement de labinter ainsi que la tuile supplémentaire et la position du joueur */
            else{
                mouvement->insert = 1;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
            }
        }
    }
    
    /* insert = 2 */
    for (int i = 1;i < tx;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((ancienmouv.insert == 3) && (ancienmouv.number == i)){
            continue;
        }

        for (int k = 0;k<4;k++){
            /* On intialise le labyrinthe avec toutes les cases item à 0 et on collecte les coordonnées d'arrivée */
            resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);

            /* Si le trésor est à l'extrémité et peut être poussée hors du labyrinthe, on skip ce coup */
            if ((arrivee[1] == i) && (arrivee[0] == ty-1)){
                continue;
            }
            tuile_suppinter = donnees.tuile_supplementaire;
            tuile_suppinter.tileI = 0;
            depart[0] = donnees.joueur1.y;
            depart[1] = donnees.joueur1.x;

            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[ty-1][i];
            for (int m = ty-1;m > 0;m--){
                labinter[m][i] = labinter[m-1][i];
            }
            rotation(k,&tuile_suppinter);
            labinter[0][i] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur et du trésor dans le cas où ils soient sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[0] = deplacementJoueur(donnees.joueur1.y,donnees.joueur1.x,i,1,ty-1,0);
            arrivee[0] = deplacementJoueur(arrivee[0],arrivee[1],i,1,ty-1,0);

            /* Si le trésor est dans la tuile supplémentaire, on prend pour coord d'arrivée celle de la tuile insérée */
            if (donnees.joueur1.nextI == donnees.tuile_supplementaire.tileI){
                arrivee[0] = 0;
                arrivee[1] = i;
            }

            /* On vérifie si un chemin existe */
            chemin = expansion(tx,ty,labinter,depart,arrivee);
            
            /* Si un chemin existe on modifie le mouvement accordément puis on termine la fonction */
            if (chemin == -1){
                mouvement->insert = 2;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
                return 1;
            }
            /* Sinon on réinitialise l'arrangement de labinter ainsi que la tuile supplémentaire et la position du joueur */
            else{
                mouvement->insert = 2;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
            }
        }
    }
    
    /* insert = 3 */
    for (int i = 1;i < tx;i = i + 2){
        
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((ancienmouv.insert == 2) && (ancienmouv.number == i)){
            continue;
        }

        for (int k = 0;k<4;k++){
            /* On intialise le labyrinthe avec toutes les cases item à 0 et on collecte les coordonnées d'arrivée */
            resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);

            /* Si le trésor est à l'extrémité et peut être poussée hors du labyrinthe, on skip ce coup */
            if ((arrivee[1] == i) && (arrivee[0] == 0)){
                continue;
            }
            tuile_suppinter = donnees.tuile_supplementaire;
            tuile_suppinter.tileI = 0;
            depart[0] = donnees.joueur1.y;
            depart[1] = donnees.joueur1.x;

            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[0][i];
            for (int m = 0;m < ty-1;m++){
                labinter[m][i] = labinter[m+1][i];
            }
            rotation(k,&tuile_suppinter);
            labinter[ty-1][i] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur et du trésor dans le cas où ils soient sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[0] = deplacementJoueur(donnees.joueur1.y,donnees.joueur1.x,i,-1,0,ty-1);
            arrivee[0] = deplacementJoueur(arrivee[0],arrivee[1],i,-1,0,ty-1);

            /* Si le trésor est dans la tuile supplémentaire, on prend pour coord d'arrivée celle de la tuile insérée */
            if (donnees.joueur1.nextI == donnees.tuile_supplementaire.tileI){
                arrivee[0] = ty-1;
                arrivee[1] = i;
            }

            /* On vérifie si un chemin existe */
            chemin = expansion(tx,ty,labinter,depart,arrivee);

            /* Si un chemin existe on modifie le mouvement accordément puis on termine la fonction */
            if (chemin == -1){
                mouvement->insert = 3;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
                return 1;
            }
            /* Sinon on réinitialise l'arrangement de labinter ainsi que la tuile supplémentaire et la position du joueur */
            else{
                mouvement->insert = 3;
                mouvement->number = i;
                mouvement->rotation = k;
                mouvement->x = arrivee[1];
                mouvement->y = arrivee[0];
            }
        }
    }
    return 0;
}

