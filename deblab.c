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
    int tailleX,tailleY;
}t_labyrinthe;

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

int init_jeu(char nom_jeu[50],int tailleX,int tailleY,int * lab,int case_N,int case_E,int case_S,int case_O,int case_I){
    int numero_joueur_depart;
    
    waitForLabyrinth("TRAINING DONTMOVE timeout=1000",nom_jeu,&tailleX,&tailleY);
    
    lab = malloc(5*tailleX*tailleY*sizeof(int));
    
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    return numero_joueur_depart;
}

int rotation(int angle,t_tuile *tile){
    int inter = tile->tileN;
    if (angle == 0){
        return 0;
    }
    else{
        tile->tileN = tile->tileW;
        tile->tileW = tile->tileS;
        tile->tileS = tile->tileE;
        tile->tileE = inter;
        rotation(angle-1,tile);
    }
    return 0;
}

void MaJDonnees1(t_move mouvement,t_labyrinthe * donnees,t_tuile * laby[X][Y],int num_depart){
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
    int tailleX = 0,tailleY = 0,case_N = 0,case_E = 0,case_S = 0,case_O = 0,case_I = 0;
    char nom_jeu[50];
    int * lab = malloc(0);
    t_move mouv_joueur,mouv_bot;

    /* Connection au serveur et récupération des données */
    connectToServer("172.105.76.204",1234,"DONTMOVE");
    numero_joueur_depart = init_jeu(nom_jeu,tailleX,tailleY,lab,case_N,case_E,case_S,case_O,case_I);
    
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
    }
    closeConnection();
    return 0;
}