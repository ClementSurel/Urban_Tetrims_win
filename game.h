#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED


#define MOVING_PERIOD           80
#define MOVING_PERIOD_START     160

#define HARD_DROP_PERIOD        50

#define LOCK_DELAY              500
#define MAX_MOVES_ON_STACK       10

#define SCORE_MAX               999999999


typedef struct Position Position;
typedef struct GameElements GameElements;

#include "animation.h"
#include "linked_list.h"

enum { TETRIM_I, TETRIM_O, TETRIM_T, TETRIM_L, TETRIM_J, TETRIM_Z, TETRIM_S };

enum {  BLOCK_VOID, BLOCK_ACTIVE,
        BLOCK_YELLOW, BLOCK_RED, BLOCK_CYAN, BLOCK_GREEN, BLOCK_BLUE, BLOCK_PURPLE, BLOCK_ORANGE, };

typedef enum Direction
{   DIR_LEFT,
    DIR_RIGHT
} Direction;

struct Position
{
    int i;
    int j;
};

struct GameElements
{
    Uint32 gMap[NB_BLOCK_X][NB_BLOCK_Y]; /* The map of the plafield */
    LNK_List *bag; /* List of the next tetriminoes */
    Uint8 tetrimActive; /* Boolean */
    int actualTetrim;
    int dimension; /* dimension of the actual Tetrimino */
    Position block1; /* A blocks of reference used to move or to turn the tetrimino.
                        Tetrimino can be seen as an object put in a virtual square of the tetrimino dimension.
                        block1 is the upper left block of this square.
                        Warning : in some situations, block1 can be outside of the playfield */
    Uint16 rotationState;
    Uint32 nextTetrimMap[4][4]; /* Appears on the right panel */
    int nextTetrim;
    Uint32 score;
    int level;
    int nbCompleteLines;
};


Uint8 initGameElements (GameElements *gameElm);

void freeGameElements (GameElements *gameElm);

/** \brief The main function of the game. The one that calls all the other **/
Uint8 playGame (SDL_Surface *screen, Sprites*);

/** The function returns a boolean : 1 if a new tetrimino has been generated successfully, 0 if not **/
Uint8 generateNewTetrim (GameElements *gameElm);

/** The function returns a boolean : 1 if the tetrimino is falling, 0 if it touches the stack **/
Uint8 tetrimFalls (GameElements *gameElm);

/** The tetrimino moves to the left or to the right when player asks it **/
void tetrimMoves (GameElements *gameElm, Direction dir);

void tetrimRotates (GameElements *gameElm);

/** \brief After 0.5 second on the ground or on the stack, the active tetrimino is locked (i.e. becomes inactive
 * \return the number of complete lines */
Uint8 locksTetrim (GameElements *gameElm);

/** Returns a boolean : 1 if there is at least one complete line, 0 otherwise **/
Uint8 checkCompleteLines (GameElements *gameElm);

/** Paints a tetrimino in a 4x4 2D array. This function is called by generateNewTetrim and turnBloc. **/
void putTetrim (Uint32 field[4][4], int tetrim);

#endif
