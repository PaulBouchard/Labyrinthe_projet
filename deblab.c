#include <stdlib.h>
#include <stdio.h>
#include "labyrinthAPI.h"
#include "clientAPI.h"

int main(void){
    int numero_joueur;
    int tailleX,tailleY;
    int *lab = malloc(5*tailleX*tailleY*sizeof(int));
    int case_N,case_E,case_S,case_O,case_I;
    char nom_jeu[50];
    int intermediaire_typenum;

    
    connectToServer("172.105.76.204",1234,"DONTMOVE");
    waitForLabyrinth("TRAINING DONTMOVE",nom_jeu,&tailleX,&tailleY);
    numero_joueur = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    printLabyrinth();
    
    printf("\n%d\n",numero_joueur);
    
    t_move mouv_joueur;
    int num_mouv_joueur = 0;
    
    t_move mouv_bot;
    if (numero_joueur == 1){
        getMove(&mouv_bot);
    }

    for (int i =0;i<3;i++){
        printf("\nEntrez un chiffre entre 0 et 3 indiquant dans quelle face la piece sera insérée (0=gauche,1=droite,haut=2,bas=3)\n");
        scanf("%d",&intermediaire_typenum);
        mouv_joueur.insert = intermediaire_typenum;
        
        printf("\nEntrez le numéro de la ligne ou de la colonne dans laquelle ira la pièce\n");
        scanf("%d",&(mouv_joueur.number));
        
        printf("\nEntrez un chiffre entre 0 et 3 indiquant la rotation appliquée à la pièce en sens horaire (0=rien,1=90degrés,..\n");
        scanf("%d",&(mouv_joueur.rotation));
        
        printf("\nEntrez les coordonnées de la case où vous voulez aller (x y)\n");
        scanf("%d %d",&(mouv_joueur.x),&(mouv_joueur.y));

        num_mouv_joueur = sendMove(&mouv_joueur);
        getMove(&mouv_bot);
        printLabyrinth();

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
        
    }
    closeConnection();
    return 0;
}