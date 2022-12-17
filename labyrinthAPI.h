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


#ifndef __API_CLIENT_LABYRINTH__
#define __API_CLIENT_LABYRINTH__
#include "clientAPI.h"


typedef enum
{
	INSERT_LINE_LEFT = 	0,
	INSERT_LINE_RIGHT = 1,
	INSERT_COLUMN_TOP = 2,
	INSERT_COLUMN_BOTTOM = 3
} t_insertion;


/*
A move is a composed of:
- a type of insertion (that can be INSERT_LINE_LEFT, INSERT_LINE_RIGHT, INSERT_COLUMN_UP, or INSERT_COLUMN_DOWN)
- the line or column number for the insertion
- the rotation of the tile to be inserted (from 0 to 3 clockwise quarters)
- a tuple (x,y) that indicates where to move
- info on the new tile (if it has a wall on North, East, South, West, and it's item number)
- next item for the player who has played
*/
typedef struct
{
	t_insertion insert; 	/* type of the insertion */
	int number;				/* column or line number */
	int rotation;
	int x, y; 				/* coordinate where to move */
	int tileN, tileE, tileS, tileW, tileItem;	/* new tile (set by playMove or getMove) */
	int nextItem;			/* next item for the player (set by playMove or getMove) */
} t_move;



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
void connectToServer(const char* serverName, int port, char* name);


/* ----------------------------------
 * Close the connection to the server
 * to do, because we are polite
 *
 * Parameters: None
*/
void closeConnection();


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
 *
 */
void waitForLabyrinth(const char* gameType, char* labyrinthName, int* sizeX, int* sizeY);


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
 int getLabyrinth(int* lab, int* tileN, int* tileE, int* tileS, int* tileW, int* tileItem);



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
 t_return_code getMove(t_move* move);



/* -----------
 * Send a move
 *
 * Parameters:
 * - move: a move
 *
 * Returns a return_code
 * NORMAL_MOVE for normal move,
 * WINNING_MOVE for a winning move, -1
 * LOSING_MOVE for a losing (or illegal) move
 * this code is relative to your programm (WINNING_MOVE if YOU win, ...)
 */
t_return_code sendMove(t_move* move);



/* ----------------------
 * Display the labyrinth
 * in a pretty way (ask the server what to print)
 */
void printLabyrinth();



/* ----------------------------
 * Send a comment to the server
 *
 * Parameters:
 * - comment: (string) comment to send to the server (max 100 char.)
 */
void sendComment(const char* comment);



#endif
