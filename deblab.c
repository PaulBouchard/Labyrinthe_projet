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
    scanf("%d,%d,%d,%d %d",&intermediaire_typenum,&mouvement->number,&mouvement->rotation,&mouvement->x,&mouvement->y);
    mouvement->insert = intermediaire_typenum;
}

void init_type(t_labyrinthe * laby,int case_N,int case_E,int case_S,int case_O,int case_I,int * lab,int tx,int ty,t_tuile * labyrinthe,t_tuile look[ty][tx]){
    /* Initialisation de la position et du prochain trésor à trouver du joueur 1 */
    laby->joueur1.x = 0;
    laby->joueur1.y = 0;
    laby->joueur1.nextI = 1;

    /* Initialisation de la position et du prochain trésor à trouver du joueur 2 */
    laby->joueur2.x = tx - 1;
    laby->joueur2.y = ty - 1;
    laby->joueur2.nextI = 1;

    /* Initialisation de la tuile supplémentaire */
    laby->tuile_supplementaire.tileN = case_N;
    laby->tuile_supplementaire.tileE = case_E;
    laby->tuile_supplementaire.tileS = case_S;
    laby->tuile_supplementaire.tileW = case_O;
    laby->tuile_supplementaire.tileI = case_I;

    /* Remplissage du labyrinthe via un intermédiaire */
    int j = 0;
    for (int i = 0;i < 5*tx*ty;i = i+5){
        labyrinthe[j].tileN = lab[i];
        labyrinthe[j].tileE = lab[i+1];
        labyrinthe[j].tileS = lab[i+2];
        labyrinthe[j].tileW = lab[i+3];
        labyrinthe[j].tileI = lab[i+4];
        j = j + 1;
    }

    int n=0;
    int m=0;
    for (int k = 0;k<tx*ty;k++){
        if (n==tx){
            n=0;
            m=m+1;
        }
        look[m][n].tileN = labyrinthe[k].tileN;
        look[m][n].tileE = labyrinthe[k].tileE;
        look[m][n].tileS = labyrinthe[k].tileS;
        look[m][n].tileW = labyrinthe[k].tileW;
        look[m][n].tileI = labyrinthe[k].tileI;
        n=n+1;
    }
}

int rotation(int angle,t_tuile *tile){
    int inter = tile->tileN;    // valeur intermédiaire pour le changement de valeurs lors de la rotation
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
    return 0;
}

void MaJDonnees1(t_move mouvement,t_labyrinthe * donnees,t_tuile * laby[X][Y],int num_depart,int tx,int ty){
    t_tuile inter;
    
    /* Récupération des positions et du prochain trésor */
    if (num_depart == 1){
        donnees->joueur2.x = mouvement.x;
        donnees->joueur2.y = mouvement.y;
        donnees->joueur2.nextI = mouvement.nextItem;
    }
    if (num_depart == 0){
        donnees->joueur1.x = mouvement.x;
        donnees->joueur1.y = mouvement.y;
        donnees->joueur1.nextI = mouvement.nextItem;
    }

    /* Modification du labyrinthe selon la tuile insérée et le côté choisi */
    if (mouvement.insert == 0){
        inter = *laby[X-1][mouvement.number];
        for (int i = X-1;i > 0; i--){
            *laby[i][mouvement.number] = *laby[i-1][mouvement.number];
        }
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        *laby[0][mouvement.number] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;
    }
    else if (mouvement.insert == 1){
        inter = *laby[0][mouvement.number];
        for (int i = 0;i < X-1; i++){
            *laby[i][mouvement.number] = *laby[i+1][mouvement.number];
        }
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        *laby[X-1][mouvement.number] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;
    }
    else if (mouvement.insert == 2){
        inter = *laby[mouvement.number][Y-1];
        for (int i = Y-1;i > 0; i--){
            *laby[mouvement.number][i] = *laby[mouvement.number][i-1];
        }
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        *laby[mouvement.number][0] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;
    }
    else if (mouvement.insert == 3){
        inter = *laby[mouvement.number][0];
        for (int i = 0;i < Y-1; i++){
            *laby[mouvement.number][i] = *laby[mouvement.number][i+1];
        }
        rotation(mouvement.rotation,&donnees->tuile_supplementaire);
        *laby[mouvement.number][Y-1] = donnees->tuile_supplementaire;
        donnees->tuile_supplementaire = inter;
    }
}


int main(void){
    /* Déclaration des variables */
    int numero_joueur_depart,num_mouv_joueur,num_mouv_bot;
    int tailleX,tailleY,case_N,case_E,case_S,case_O,case_I;
    char nom_jeu[50];
    t_move mouv_joueur,mouv_bot;

    /* Connection au serveur et récupération des données */
    connectToServer("172.105.76.204",1234,"DONTMOVE");
    waitForLabyrinth("TRAINING DONTMOVE timeout=1000 seed=0xf653ce",nom_jeu,&tailleX,&tailleY);
    
    int * lab = malloc(5*tailleX*tailleY*sizeof(int));
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    t_tuile * labyrinthe = malloc(5*tailleX*tailleY*sizeof(int));
    
    printf("tailleX = %d\ntailleY = %d\nseed = %s\n",tailleX,tailleY,nom_jeu);
    
    t_tuile (*look)[tailleX];
    look = malloc(sizeof(*look)*tailleY);
    look = malloc(sizeof(int[tailleY][tailleX]));
    
    t_labyrinthe donnees;
    init_type(&donnees,case_N,case_E,case_S,case_O,case_I,lab,tailleX,tailleY,labyrinthe,look);    

    printLabyrinth();

    /* Début de partie */
    /*while (1){
        
        if (numero_joueur_depart == 1){
            num_mouv_bot = getMove(&mouv_bot);
            
            demande_coup_joueur(&mouv_joueur);
            
            num_mouv_joueur = sendMove(&mouv_joueur);

            printLabyrinth();
        }
        else if(numero_joueur_depart == 0){
            printLabyrinth();
            
            demande_coup_joueur(&mouv_joueur);
            
            num_mouv_joueur = sendMove(&mouv_joueur);

            num_mouv_bot = getMove(&mouv_bot);
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
    }*/
    closeConnection();
    return 0;
}