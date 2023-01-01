#include <stdlib.h>
#include <stdio.h>
#include "labyrinthAPI.h"
#include "clientAPI.h"

#define X 10
#define Y 10

typedef struct{
    int x,y;
    int nextI;
}t_position;

typedef struct{
    int tileN,tileS,tileW,tileE;
    int tileI;
}t_tuile;

typedef struct{
    t_position joueur1;
    t_position joueur2;
    t_tuile tuile_supplementaire;
}t_labyrinthe;

void demande_coup_joueur(t_move * mouvement){
    int intermediaire_typenum;
    /*printf("Entrez un chiffre entre 0 et 3 indiquant par quel côté la piece sera insérée (0=O,1=E,2=N,S=3)\n");
    
        
    printf("Entrez le numéro de la ligne ou de la colonne dans laquelle ira la pièce\n");
    scanf("%d",&mouvement->number);
        
    printf("Entrez un chiffre entre 0 et 3 indiquant la rotation appliquée à la pièce en sens horaire\n");
    scanf("%d",&mouvement->rotation);

    printf("Entrez les coordonnées de la case où vous voulez aller (x y)\n");
    scanf("%d %d",&mouvement->x,&mouvement->y);*/

    printf("Coup -> ");
    scanf("%d %d %d %d %d",&intermediaire_typenum,&mouvement->number,&mouvement->rotation,&mouvement->x,&mouvement->y);
    mouvement->insert = intermediaire_typenum;
}

void init_type(t_labyrinthe * donnees,int case_N,int case_E,int case_S,int case_O,int case_I,int * lab,int tx,int ty,t_tuile look[ty][tx]){
    /* Initialisation de la position et du prochain trésor à trouver du joueur 1 */
    donnees->joueur1.x = 0;
    donnees->joueur1.y = 0;
    donnees->joueur1.nextI = 1;

    /* Initialisation de la position et du prochain trésor à trouver du joueur 2 */
    donnees->joueur2.x = tx - 1;
    donnees->joueur2.y = ty - 1;
    donnees->joueur2.nextI = 1;

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

void insertionLigne(int ligne,int max,int min,int plusmoins,int angle_rot,int tx,int ty,t_tuile laby[ty][tx],t_tuile * tuilesupp){
    t_tuile inter = laby[ligne][max];

    /* Mise à jour d'une ligne du labyrinthe */
    for (int i = max;i==min;i = i + plusmoins){
        laby[ligne][i] = laby[ligne][i+plusmoins];
    }

    /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la ligne */
    rotation(angle_rot,tuilesupp);
    laby[ligne][min] = *tuilesupp;
    *tuilesupp = inter;
}

void insertionColonne(int colonne,int max,int min,int plusmoins,int angle_rot,int tx,int ty,t_tuile laby[ty][tx],t_tuile * tuilesupp){
    t_tuile inter = laby[max][colonne];

    /* Mise à jour d'une colonne du labyrinthe */
    for (int i = max;i == min;i = i + plusmoins){
        laby[i][colonne] = laby[i+plusmoins][colonne];
    }
    
    /* Rotation de la tuile supplémentaire et ajout de cette dernière dans la colonne */
    rotation(angle_rot,tuilesupp);
    laby[min][colonne] = *tuilesupp;
    *tuilesupp = inter;
}

int deplacementJoueur(int positionbouge,int positionbougepas,int rangee,int plusmoins,int max,int min){
    if (positionbougepas == rangee){
        if (positionbouge == max){
            return min;
        }
        else{
            return (positionbouge + plusmoins);
        }
    }
}

void MaJDonnees(t_move mouvement,t_labyrinthe * donnees,int ty,int tx,t_tuile laby[ty][tx],int num_joueur){
    /* Modification du labyrinthe selon la tuile insérée et le côté choisi et modif de la position des joueurs*/
    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des lignes vers la droite */
    if (mouvement.insert == 0){
        /* Insertion de la tuile dans la ligne */
        insertionLigne(mouvement.number,tx-1,0,-1,mouvement.rotation,tx,ty,laby,&donnees->tuile_supplementaire);

        /* Mise à jour des position des joueurs au cas où ils soient sur la ligne modifiée */
        donnees->joueur1.x = deplacementJoueur(donnees->joueur1.x,donnees->joueur1.y,mouvement.number,1,tx-1,0);
        donnees->joueur2.x = deplacementJoueur(donnees->joueur2.x,donnees->joueur2.y,mouvement.number,1,tx-1,0);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des lignes vers la gauche */
    else if (mouvement.insert == 1){
        /* Insertion de la tuile dans la ligne */
        insertionLigne(mouvement.number,0,tx-1,1,mouvement.rotation,tx,ty,laby,&donnees->tuile_supplementaire);

        /* Mise à jour des position des joueurs au cas où ils soient sur la ligne modifiée */
        donnees->joueur1.x = deplacementJoueur(donnees->joueur1.x,donnees->joueur1.y,mouvement.number,-1,0,tx-1);
        donnees->joueur2.x = deplacementJoueur(donnees->joueur2.x,donnees->joueur2.y,mouvement.number,-1,0,tx-1);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des colonnes vers le bas */
    else if (mouvement.insert == 2){
        /* Mise à jour d'une colonne du labyrinthe */
        insertionColonne(mouvement.number,ty-1,0,-1,mouvement.rotation,tx,ty,laby,&donnees->tuile_supplementaire);

        /* Mise à jour des position des joueurs au cas où ils soient sur la colonne modifiée */
        donnees->joueur1.y = deplacementJoueur(donnees->joueur1.y,donnees->joueur1.x,mouvement.number,1,ty-1,0);
        donnees->joueur2.y = deplacementJoueur(donnees->joueur2.y,donnees->joueur2.x,mouvement.number,1,ty-1,0);
    }

    /* Modif du labyrinthe et des positions en "poussant" les tuiles d'une des colonnes vers le haut */
    else if (mouvement.insert == 3){
        /* Mise à jour d'une colonne du labyrinthe */
        insertionColonne(mouvement.number,0,ty-1,1,mouvement.rotation,tx,ty,laby,&donnees->tuile_supplementaire);

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

int coup_auto(t_move * mouvement,t_labyrinthe donnees,int tx,int ty,t_tuile laby[ty][tx]){
    /* Déclaration des variables (intermédiaire pour la plupart) */
    t_tuile tuile_suppinter = donnees.tuile_supplementaire;
    t_tuile labinter[ty][tx];
    t_tuile inter;
    int coordIi,coordIj;
    int chemin;

    /* Copie du labyrinthe et récupération des coordonnées du trésor à trouver */
    for (int i = 0;i < ty;i++){
        for (int j = 0;j < tx;j++){
            labinter[i][j] = laby[i][j];
            if (donnees.joueur1.nextI == laby[i][j].tileI){
                coordIi = i;
                coordIj = j;
                continue;
            }
            labinter[i][j].tileI = 0;
        }
    }
    labinter[coordIi][coordIj].tileI = -1;

    /* Cas où insert = 0 et 1 */
    for (int i = 1;i < ty;i = i + 2){
        for (int k = 0;k < 4;k++){
            insertionLigne(i,tx-1,0,-1,k,tx,ty,labinter,&tuile_suppinter);
            donnees->joueur1.x = deplacementJoueur(donnees->joueur1.x,donnees->joueur1.y,mouvement.number,1,tx-1,0);
            chemin = expansion();
            if (chemin ==)
        }
    }
    return 0;
}

int expansion(int tx,int ty,t_tuile laby[ty][tx],t_labyrinthe donnees){
    /* Déclaration des variables (intermédiaire pour la plupart) */
    t_tuile tuile_suppinter = donnees.tuile_supplementaire;
    t_tuile labinter[ty][tx];
    t_tuile inter;
    int arrivee[2];

    /* Copie du labyrinthe et récupération des coordonnées du trésor à trouver */
    for (int i = 0;i < ty;i++){
        for (int j = 0;j < tx;j++){
            labinter[i][j] = laby[i][j];
            if (donnees.joueur1.nextI == laby[i][j].tileI){
                arrivee[0] = i;
                arrivee[1] = j;
                continue;
            }
            labinter[i][j].tileI = 0;
        }
    }
    labinter[arrivee[0]][arrivee[1]].tileI = -1;
}

int main(void){
    /* Déclaration des variables */
    int numero_joueur_depart,num_mouv_joueur,num_mouv_bot;
    int tailleX,tailleY,case_N,case_E,case_S,case_O,case_I;
    char nom_jeu[50];
    t_move mouv_joueur,mouv_bot;
    t_labyrinthe donnees;

    /* Connection au serveur et récupération des tailles */
    connectToServer("172.105.76.204",1234,"DONTMOVE");
    waitForLabyrinth("TRAINING DONTMOVE timeout=1000 seed=0xf653ce",nom_jeu,&tailleX,&tailleY);
    //printf("tailleX = %d\ntailleY = %d\nseed = %s\n",tailleX,tailleY,nom_jeu);
    
    /* Récupération du labyrinthe et de la case supplémentaire */
    int * lab = malloc(5*tailleX*tailleY*sizeof(int));
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    /* Initialisation du jeu avec les données de départ */
    t_tuile labyrinthe[tailleY][tailleX];
    init_type(&donnees,case_N,case_E,case_S,case_O,case_I,lab,tailleX,tailleY,labyrinthe);    

    /* Début de partie */
    while (1){
        
        if (numero_joueur_depart == 1){
            num_mouv_bot = getMove(&mouv_bot);
            
            printLabyrinth();
            
            demande_coup_joueur(&mouv_joueur);
            
            num_mouv_joueur = sendMove(&mouv_joueur);
        }
        else if(numero_joueur_depart == 0){
            printLabyrinth();
            
            demande_coup_joueur(&mouv_joueur);
            
            num_mouv_joueur = sendMove(&mouv_joueur);
            
            MaJDonnees(mouv_joueur,&donnees,tailleY,tailleX,labyrinthe,0);
            
            num_mouv_bot = getMove(&mouv_bot);
            
            MaJDonnees(mouv_bot,&donnees,tailleY,tailleX,labyrinthe,1);
        }       

        if (num_mouv_joueur == 1){
            printf("Vous avez gagné");
            closeConnection();
            return 0;
        }
        else if (num_mouv_joueur == -1){
            printf("Vous avez fait un mouvement illégal, vous perdez, le bot gagne");
            closeConnection();
            return 0;
        }

        if (num_mouv_bot == 1){
            printf("Vous avez perdu, le robot a gagné");
            closeConnection();
            return 0;
        }
        else if (num_mouv_bot == -1){
            printf("Le robot a fait un mouvement illégal, vous gagnez, le bot perd");
            closeConnection();
            return 0;
        }   
    }
    closeConnection();
    printf("AU REVOIR\n");
    return 0;
}