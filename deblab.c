#include <stdlib.h>
#include <stdio.h>
#include "labyrinthAPI.h"
#include "clientAPI.h"
#include "fonclab.h"

int main(void){
    /* Déclaration des variables */
    int numero_joueur_depart,num_mouv_joueur,num_mouv_bot;
    int tailleX,tailleY,case_N,case_E,case_S,case_O,case_I;
    char nom_jeu[50];
    t_position liste_position[500];
    t_move mouv_joueur,mouv_bot;
    t_labyrinthe donnees;

    /* Connection au serveur et récupération des tailles */
    connectToServer("172.105.76.204",1234,"joueurtest");
    waitForLabyrinth("TRAINING BASIC timeout=1000",nom_jeu,&tailleX,&tailleY);
    printf("tailleX = %d\ntailleY = %d\nseed = %s\n",tailleX,tailleY,nom_jeu);
    
    /* Récupération du labyrinthe et de la case supplémentaire */
    int * lab = malloc(5*tailleX*tailleY*sizeof(int));
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    /* Initialisation du jeu avec les données de départ */
    t_tuile labyrinthe[tailleY][tailleX];
    int indicemouv = 1;
    init_type(&donnees,case_N,case_E,case_S,case_O,case_I,lab,tailleX,tailleY,labyrinthe,numero_joueur_depart,liste_position);    
    

    /* Début de partie */
    while (1){
        /* Si c'est le joueur 1 qui commence */
        if (numero_joueur_depart == 0){
            printLabyrinth();
            
            coup_auto(&mouv_joueur,donnees,tailleX,tailleY,labyrinthe,mouv_bot,liste_position,indicemouv);
            num_mouv_joueur = sendMove(&mouv_joueur);
            printf("[%d %d %d %d %d]\n\n",mouv_joueur.insert,mouv_joueur.number,mouv_joueur.rotation,mouv_joueur.x,mouv_joueur.y);
            if (num_mouv_joueur != 0){
                if (num_mouv_joueur == 1){
                    printf("Vous avez gagné\n");
                    closeConnection();
                    return 0;
                }
                if (num_mouv_joueur == -1){
                    printf("Vous avez perdu -> mouvement interdit\n");
                    closeConnection();
                    return 0;
                }
            }
            MaJDonnees(mouv_joueur,&donnees,tailleX,tailleY,labyrinthe,0,liste_position,indicemouv);
            indicemouv = indicemouv + 1;
                        
            num_mouv_bot = getMove(&mouv_bot);
            if (num_mouv_bot != 0){
                if (num_mouv_bot == 1){
                    printf("Vous avez perdu\n");
                    closeConnection();
                    return 0;
                }
                if (num_mouv_bot == -1){
                    printf("Vous avez gagné -> mouvement interdit du bot\n");
                    closeConnection();
                    return 0;
                }
            }
            MaJDonnees(mouv_bot,&donnees,tailleX,tailleY,labyrinthe,1,liste_position,indicemouv);
        }
        
        else if (numero_joueur_depart == 1){
            
            num_mouv_bot = getMove(&mouv_bot);
            if (num_mouv_bot != 0){
                if (num_mouv_bot == 1){
                    printf("Vous avez perdu\n");
                    closeConnection();
                    return 0;
                }
                if (num_mouv_bot == -1){
                    printf("Vous avez gagné -> mouvement interdit du bot\n");
                    closeConnection();
                    return 0;
                }
            }
            MaJDonnees(mouv_bot,&donnees,tailleX,tailleY,labyrinthe,1,liste_position,indicemouv);

            printLabyrinth();

            coup_auto(&mouv_joueur,donnees,tailleX,tailleY,labyrinthe,mouv_bot,liste_position,indicemouv);
            num_mouv_joueur = sendMove(&mouv_joueur);
            if (num_mouv_joueur != 0){
                if (num_mouv_joueur == 1){
                    printf("Vous avez gagné\n");
                    closeConnection();
                    return 0;
                }
                if (num_mouv_joueur == -1){
                    printf("Vous avez perdu -> mouvement interdit\n");
                    closeConnection();
                    return 0;
                }
            }
            MaJDonnees(mouv_joueur,&donnees,tailleX,tailleY,labyrinthe,0,liste_position,indicemouv);
            indicemouv = indicemouv + 1;
        }
    }
    closeConnection();
    printf("AU REVOIR\n");
    return 0;
}