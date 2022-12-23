#include <stdlib.h>
#include <stdio.h>
#include "labyrinthAPI.h"
#include "clientAPI.h"

int main(void){
    int numero_joueur_depart;
    int tailleX,tailleY;
    int case_N,case_E,case_S,case_O,case_I;
    char nom_jeu[50];
    int intermediaire_typenum;

    connectToServer("172.105.76.204",1234,"DONTMOVE");
    waitForLabyrinth("TRAINING DONTMOVE key=1000",nom_jeu,&tailleX,&tailleY);
    int *lab = malloc(5*tailleX*tailleY*sizeof(int));
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    printf("\n%d\n",numero_joueur_depart);
    
    t_move mouv_joueur;
    int num_mouv_joueur = 0;
    
    t_move mouv_bot;
    int num_mouv_bot = 0;
    
    if (numero_joueur_depart == 1){
        getMove(&mouv_bot);
        printLabyrinth();
    }
    else{
        printLabyrinth();
    }

    while (1){
        printf("Entrez un chiffre entre 0 et 3 indiquant par quel côté la piece sera insérée (0=O,1=E,2=N,S=3)\n");
        scanf("%d",&intermediaire_typenum);
        mouv_joueur.insert = intermediaire_typenum;
        
        printf("Entrez le numéro de la ligne ou de la colonne dans laquelle ira la pièce\n");
        scanf("%d",&(mouv_joueur.number));
        
        printf("Entrez un chiffre entre 0 et 3 indiquant la rotation appliquée à la pièce en sens horaire\n");
        scanf("%d",&(mouv_joueur.rotation));
        
        printf("Entrez les coordonnées de la case où vous voulez aller (x y)\n");
        scanf("%d %d",&(mouv_joueur.x),&(mouv_joueur.y));

        num_mouv_joueur = sendMove(&mouv_joueur);
        printLabyrinth();
        //getLabyrinth(lab,&(mouv_joueur.tileN),&(mouv_joueur.tileE),&(mouv_joueur.tileS),&(mouv_joueur.tileW),&(mouv_joueur.tileItem));

        num_mouv_bot = getMove(&mouv_bot);
        printLabyrinth();
        //getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);

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
    return 0;
}