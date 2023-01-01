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
    int last_insert;
    int last_number;
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

    /* Pas de coup encore fait donc tout est nul */
    donnees->last_insert = 0;
    donnees->last_number = 0;

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

    donnees->last_insert = mouvement.insert;
    donnees->last_number = mouvement.number;
}

void resetLabyrinth(int tx,int ty,t_tuile labareset[ty][tx],t_tuile exempleLab[ty][tx],int arrivee[2],int tresor){
    for (int i = 0;i < ty;i++){
        for (int j = 0;j < tx;j++){
            labareset[i][j] = exempleLab[i][j];
            if (tresor == exempleLab[i][j].tileI){
                arrivee[0] = i;
                arrivee[1] = j;
            }
            labareset[i][j].tileI = -exempleLab[i][j].tileI;
        }
    }
}

int expansion(int tx,int ty,t_tuile laby[ty][tx],int depart[2],int arrivee[2]){
    int r = 1;
    laby[depart[0]][depart[1]].tileI = r;
    int parcours_case = 0;
    laby[arrivee[0]][arrivee[1]].tileI = 0;
    int max = 0;

    while (laby[arrivee[0]][arrivee[1]].tileI == 0){
        for (int i = 0;i < ty; i++){
            for (int j = 0;j < tx; j++){
                if ((i == depart[0])&&(j == depart[1])){
                    continue;
                }
                else{
                    if (laby[i][j].tileI <= 0){
                        if ((laby[i+1][j].tileI == r) && (i<ty-1) && (laby[i][j].tileS == 0) && (laby[i+1][j].tileN == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i-1][j].tileI == r) && (i>0) && (laby[i][j].tileN == 0) && (laby[i-1][j].tileS == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i][j+1].tileI == r) && (j<tx-1) && (laby[i][j].tileE == 0) && (laby[i][j+1].tileW == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                        if ((laby[i][j-1].tileI == r) && (j>0) && (laby[i][j].tileW == 0) && (laby[i][j-1].tileE == 0)){
                            laby[i][j].tileI = r + 1;
                            parcours_case = parcours_case + 1;
                        }
                    }
                }
            }
        }
        if (parcours_case == 0){
            for (int i = 0;i < ty;i++){
                for (int j = 0;j < tx;j++){
                    if (laby[i][j].tileI >= max){
                        max = laby[i][j].tileI;
                        arrivee[0] = i;
                        arrivee[1] = j;
                    }
                }
            }
            return max;
        }
        else{
            parcours_case = 0;
            r = r + 1;
        }
    }
    return -1;
}

int coup_auto(t_move * mouvement,t_labyrinthe donnees,int tx,int ty,t_tuile laby[ty][tx]){
    /* Déclaration des variables (intermédiaire pour la plupart) */
    t_tuile tuile_suppinter = donnees.tuile_supplementaire;
    t_tuile inter;
    t_tuile labinter[ty][tx];
    int depart[2] = {donnees.joueur1.y,donnees.joueur1.x};
    int arrivee[2];
    int chemin,maxdist = 0;
    int autrcoup[5];

    /* Copie du labyrinthe et récupération des coordonnées du trésor à trouver */
    resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);

    /* insert = 0 */
    for (int i = 1;i < ty;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((donnees.last_insert == 1) && (donnees.last_number == i)){
            continue;
        }
        for (int k = 0;k < 4;k++){
            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[i][tx-1];
            for (int m = tx-1;m > 0;m--){
                labinter[i][m] = labinter[i][m-1];
            }
            rotation(k,&tuile_suppinter);
            labinter[i][0] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur dans le cas où il soit sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[1] = deplacementJoueur(donnees.joueur1.x,donnees.joueur1.y,i,1,tx-1,0);
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
                if (maxdist <= chemin){
                    maxdist = chemin;
                    autrcoup[0] = 0;
                    autrcoup[1] = i;
                    autrcoup[2] = k;
                    autrcoup[3] = arrivee[0];
                    autrcoup[4] = arrivee[1];
                }
                resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);
                tuile_suppinter = donnees.tuile_supplementaire;
                depart[1] = donnees.joueur1.x;
            }
        }
    }
    
    /* insert = 1 */
    for (int i = 1;i < ty;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((donnees.last_insert == 0) && (donnees.last_number == i)){
            continue;
        }
        for (int k = 0;k < 4;k++){
            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[i][0];
            for (int m = 0;m < tx-1;m++){
                labinter[i][m] = labinter[i][m+1];
            }
            rotation(k,&tuile_suppinter);
            labinter[i][tx-1] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur dans le cas où il soit sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[1] = deplacementJoueur(donnees.joueur1.x,donnees.joueur1.y,i,-1,0,tx-1);
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
                if (maxdist <= chemin){
                    maxdist = chemin;
                    autrcoup[0] = 1;
                    autrcoup[1] = i;
                    autrcoup[2] = k;
                    autrcoup[3] = arrivee[0];
                    autrcoup[4] = arrivee[1];
                }
                resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);
                tuile_suppinter = donnees.tuile_supplementaire;
                depart[1] = donnees.joueur1.x;
            }
        }
    }
    
    /* insert = 2 */
    for (int i = 1;i < tx;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((donnees.last_insert == 3) && (donnees.last_number == i)){
            continue;
        }
        for (int k = 0;k<4;k++){
            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[ty-1][i];
            for (int m = ty-1;m > 0;m--){
                labinter[m][i] = labinter[m-1][i];
            }
            rotation(k,&tuile_suppinter);
            labinter[0][i] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur dans le cas où il soit sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[0] = deplacementJoueur(donnees.joueur1.y,donnees.joueur1.x,i,1,ty-1,0);
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
                if (maxdist <= chemin){
                    maxdist = chemin;
                    autrcoup[0] = 2;
                    autrcoup[1] = i;
                    autrcoup[2] = k;
                    autrcoup[3] = arrivee[0];
                    autrcoup[4] = arrivee[1];
                }
                resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);
                tuile_suppinter = donnees.tuile_supplementaire;
                depart[0] = donnees.joueur1.y;
            }
        }
    }
    
    /* insert = 3 */
    for (int i = 1;i < tx;i = i + 2){
        /* Pour éviter de réinsérer la tuile qui vient d'être extraite */
        if ((donnees.last_insert == 2) && (donnees.last_number == i)){
            continue;
        }
        for (int k = 0;k<4;k++){
            /* On décale toutes les tuiles d'une ligne en insérant la tuile supplémentaire rotationnée */
            inter = labinter[0][i];
            for (int m = 0;m < ty-1;m++){
                labinter[m][i] = labinter[m+1][i];
            }
            rotation(k,&tuile_suppinter);
            labinter[ty-1][i] = tuile_suppinter;
            tuile_suppinter = inter;

            /* On change la position du joueur dans le cas où il soit sur la rangée modifiée et on ensuite on cherche un chemin */
            depart[0] = deplacementJoueur(donnees.joueur1.y,donnees.joueur1.x,i,-1,0,ty-1);
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
                if (maxdist <= chemin){
                    maxdist = chemin;
                    autrcoup[0] = 3;
                    autrcoup[1] = i;
                    autrcoup[2] = k;
                    autrcoup[3] = arrivee[0];
                    autrcoup[4] = arrivee[1];
                }
                resetLabyrinth(tx,ty,labinter,laby,arrivee,donnees.joueur1.nextI);
                tuile_suppinter = donnees.tuile_supplementaire;
                depart[0] = donnees.joueur1.y;
            }
            
        }
    }
    mouvement->insert = autrcoup[0];
    mouvement->number = autrcoup[1];
    mouvement->rotation = autrcoup[2];
    mouvement->x = autrcoup[4];
    mouvement->y = autrcoup[3];
    return 0;
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
    waitForLabyrinth("TRAINING DONTMOVE timeout=1000 seed=0xf653ce start=0",nom_jeu,&tailleX,&tailleY);
    //printf("tailleX = %d\ntailleY = %d\nseed = %s\n",tailleX,tailleY,nom_jeu);
    
    /* Récupération du labyrinthe et de la case supplémentaire */
    int * lab = malloc(5*tailleX*tailleY*sizeof(int));
    numero_joueur_depart = getLabyrinth(lab,&case_N,&case_E,&case_S,&case_O,&case_I);
    
    /* Initialisation du jeu avec les données de départ */
    t_tuile labyrinthe[tailleY][tailleX];
    init_type(&donnees,case_N,case_E,case_S,case_O,case_I,lab,tailleX,tailleY,labyrinthe);    
    int num;

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
            //num = 0;
            if ((mouv_joueur.nextItem == 7)||(mouv_joueur.nextItem == 24)){
                demande_coup_joueur(&mouv_joueur);
                num_mouv_joueur = sendMove(&mouv_joueur);
            }
            else{
                coup_auto(&mouv_joueur,donnees,tailleX,tailleY,labyrinthe);
                printf("Moi -> [%d %d %d %d %d]\n",mouv_joueur.insert,mouv_joueur.number,mouv_joueur.rotation,mouv_joueur.x,mouv_joueur.y);
                num_mouv_joueur = sendMove(&mouv_joueur);
            }
            /*if (num == 1){
                //printf("%d %d %d %d %d",mouv_joueur.insert,mouv_joueur.number,mouv_joueur.rotation,mouv_joueur.x,mouv_joueur.y);
                num_mouv_joueur = sendMove(&mouv_joueur);
            }
            else{
                demande_coup_joueur(&mouv_joueur);
                num_mouv_joueur = sendMove(&mouv_joueur);
            }*/
            printf("insert et number -> %d %d\n",donnees.last_insert,donnees.last_number);
            MaJDonnees(mouv_joueur,&donnees,tailleX,tailleY,labyrinthe,0);
            printf("insert et number -> %d %d\n\n",donnees.last_insert,donnees.last_number);
            
            
            num_mouv_bot = getMove(&mouv_bot);
            printf("Bot -> [%d %d %d %d %d]\n",mouv_bot.insert,mouv_bot.number,mouv_bot.rotation,mouv_bot.x,mouv_bot.y);
            printf("insert et number -> %d %d\n",donnees.last_insert,donnees.last_number);
            MaJDonnees(mouv_bot,&donnees,tailleX,tailleY,labyrinthe,1);
            printf("insert et number -> %d %d\n\n",donnees.last_insert,donnees.last_number);
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