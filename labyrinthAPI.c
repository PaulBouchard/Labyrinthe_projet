/*
+------------------------------------------------------------------------------------+
|                                                                                    |
|                                                                                    |
|                                                                                    |
|   ___      _______  _______  __   __  ______    ___   __    _  _______  __   __    |
|  |   |    |   _   ||  _    ||  | |  ||    _ |  |   | |  |  | ||       ||  | |  |   |
|  |   |    |  |_|  || |_|   ||  |_|  ||   | ||  |   | |   |_| ||_     _||  |_|  |   |
|  |   |    |       ||       ||       ||   |_||_ |   | |       |  |   |  |       |   |
|  |   |___ |       ||  _   | |_     _||    __  ||   | |  _    |  |   |  |       |   |
|  |       ||   _   || |_|   |  |   |  |   |  | ||   | | | |   |  |   |  |   _   |   |
|  |_______||__| |__||_______|  |___|  |___|  |_||___| |_|  |__|  |___|  |__| |__|   |
|                                                                                    |
|                                                                                    |
|                                                                                    |
| API to play to classic Labyrinth game                                              |
| (with the Coding Game Server)                                                      |
|                                                                                    |
|                                                                                    |
+------------------------------------------------------------------------------------+


Authors: T. Hilaire
Licence: GPL

File: labyrinthAPI.h
   Contains the client API for the Labyrinth game
   -> based on clientAPI.h

Copyright 2021 T. Hilaire
*/


#include "clientAPI.h"
#include <stdio.h>
#include "labyrinthAPI.h"

unsigned char nX, nY; 	/* store lab size, used for getLabyrinth (the user do not have to pass them once again */


/* -------------------------------------
 * Initialize connection with the server
 * Quit the program if the connection to the server
 * cannot be established
 *
 * Parameters:
 * - serverName: (string) address of the server
 *   (it could be "localhost" if the server is run in local,
 *   or "pc4521.polytech.upmc.fr" if the server runs there)
 * - port: (int) port number used for the connection
 * - name: (string) name of this bot (max 20 characters)
 */
void connectToServer(const char* serverName, int port, char* name)
{
	connectToCGS(__FUNCTION__, serverName, port, name);
}


/* ----------------------------------
 * Close the connection to the server
 * to do, because we are polite
 *
 * Parameters:
 * None
*/
void closeConnection()
{
	closeCGSConnection(__FUNCTION__ );
}



/* ------------------------------------------------------------------------------
 * Wait for a Game, and retrieve its name and first data (array of the labyrinth
 *
 * Parameters:
 * - gameType: string (max 200 characters) type of the game we want to play (empty string for regular game)
 * - labyrinthName: string (max 50 characters), corresponds to the game name (filled by the function)
 * - sizeX, sizeY: sizes of the labyrinth
 *
 * gameType is a string like "GAME key1=value1 key2=value1 ..."
 * - It indicates the type of the game you want to plys
 *   it could be "TRAINING <BOT>" to play against bot <BOT>
 *   or "TOURNAMENT xxxx" to join the tournament xxxx
 *   or "" (empty string) to wait for an opponent (decided by the server)
 * - key=value pairs are used for options (each training player has its own options)
 *   invalid keys are ignored, invalid values leads to error
 *   the following options are common to every training player:
 *   - timeout: allows an define the timeout when training (in seconds)
 *   - 'seed': allows to set the seed of the random generator
 *   - 'start': allows to set who starts ('0' or '1')
 *
 * The bot name <BOT> could be:
 * - "PLAY_RANDOM" for a player that make random (but legal) moves
 * - "ASTAR" for a
 *
 *
 */
void waitForLabyrinth(const char* gameType, char* labyrinthName, int* sizeX, int* sizeY)
{
	char data[128];
	/* wait for a game */
	waitForGame( __FUNCTION__, gameType, labyrinthName, data);

	/* parse the data */
	sscanf( data, "%d %d", sizeX, sizeY);

	/* store the sizes, so that we can reuse them during getLabyrinth */
	nX = *sizeX;
	nY = *sizeY;
}



/* -------------------------------------
 * Get the labyrinth and tell who starts
 * It fills the char* lab with the data of the labyrinth
 * 1 if there's a wall, 0 for nothing
 *
 * Parameters:
 * - lab: the array of labyrinth (the pointer data MUST HAVE allocated with the right size !!)
 * - tile North, East, South, West and Item : extern tile (to be later inserted)
 *
 * Returns 0 if you begin, or 1 if the opponent begins
 */
int getLabyrinth(int* lab, int* tileN, int* tileE, int* tileS, int* tileW, int* tileItem)
{
	char data[4096];
	/* wait for a game */
	int ret = getGameData( __FUNCTION__, data, 4096);

	/* copy the data in the array lab
	 * the datas is a readable string of char '0' and '1'
	 * */
	char *p = data;
	int nbchar;
	for( int i=0; i<nX*nY; i++) {
		sscanf(p, "%d %d %d %d %d %n", lab, lab+1, lab+2, lab+3, lab+4, &nbchar);
		p += nbchar;
		lab += 5;
	}

	sscanf(p, "%d %d %d %d %d", tileN, tileE, tileS, tileW, tileItem);
    return ret;
}



/* ----------------------
 * Get the opponent move
 *
 * Parameters:
 * - move: a t_move variable, filled by the function
 * Returns:
 * - NORMAL_MOVE for normal move,
 * - WINNING_MOVE for a winning move, -1
 * - LOOSING_MOVE for a losing (or illegal) move
 * - this code is relative to the opponent (WINNING_MOVE if HE wins, ...)
 */
t_return_code getMove(t_move *move)
{
    char data[MAX_GET_MOVE];
    char msg[MAX_MESSAGE];

    /* get the move */
    int ret = getCGSMove(__FUNCTION__, data, msg);

	/* extract move */
	sscanf( data, "%d %d %d %d %d %d %d %d %d %d %d", &(move->insert), &(move->number), &(move->rotation),
			&(move->x), &(move->y),
			&(move->tileN), &(move->tileE), &(move->tileS), &(move->tileW), &(move->tileItem),
			&(move->nextItem));
	dispDebug(__FUNCTION__,2,"move type:%d, ret:%d",move->insert, ret);
	return ret;
}



/* -----------
 * Send a move
 *
 * Parameters:
 * - move: a move
 *
 * Returns a return_code (0 for normal move, 1 for a winning move, -1 for a losing (or illegal) move
 */
t_return_code sendMove(t_move* move)
{
    /* build the string move */
    char data[128];
    char answer[MAX_MESSAGE];
    sprintf( data, "%d %d %d %d %d", move->insert, move->number, move->rotation, move->x, move->y);
// dispDebug(__FUNCTION__,"move send : %s",data);
    /* send the move */
	int ret = sendCGSMove( __FUNCTION__, data, answer);
	/* get the new tile */
	sscanf(answer, "%d %d %d %d %d %d", &(move->tileN), &(move->tileE), &(move->tileS), &(move->tileW), &(move->tileItem), &(move->nextItem));
	return ret;
}



/* ----------------------
 * Display the labyrinth
 * in a pretty way (ask the server what to print)
 */
void printLabyrinth()
{
    printCGSGame(__FUNCTION__ );
}



/* -----------------------------
 * Send a comment to the server
 *
 * Parameters:
 * - comment: (string) comment to send to the server (max 100 char.)
 */
void sendComment(const char* comment)
{
    sendCGSComment( __FUNCTION__, comment);
}
