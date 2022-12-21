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
    
    connectToServer("172.105.76.204",1234,"DONTMOVE");
    waitForLabyrinth("TRAINING DONTMOVE",nom_jeu,&tailleX,&tailleY);
    numero_joueur = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    printLabyrinth();
    closeConnection();
    
    /*t_move mouv_bot;
    int num_mouv_bot;
    t_move mouv_joueur;
    int num_mouv_joueur;

    while (1){
        num_mouv_joueur = sendMove(mouv_joueur);

        printLabyrinth();

        if (num_mouv_joueur == 1){
            printf("Vous avez gagné");
            return 0;
        }
        else if (num_mouv_joueur == -1){
            printf("Vous avez fait un mouvement illégal, vous perdez, le bot gagne");
            return 0;
        }
        
    }*/
    
    return 0;
}