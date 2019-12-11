/******************************************************************************************************
 * constants.h defines several constants that will be used to determine the number, wide or height of *
 * different elements of the game and the interface.                                                  *
*******************************************************************************************************/

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED


#define BLOCK_SIZE      30
#define NB_BLOCK_X      10
#define NB_BLOCK_Y      (20 + 2) /* 20 lines of the playfield are visible, 2 are hidden from the player */
#define FIRST_LINE      2 /* Defines the first line of the playfield that appears on the screen */
#define GRID_WIDE       1 /* The grid is the white border inside a block */
#define BORDER          5 /* The border is the light gray/light purple border around the playfield or around the elements
                            of the interface */
#define LATERAL_PANEL       (6*BLOCK_SIZE + 2*BORDER) /* Wide of a lateral panel. The right and the left panels have the same */
#define PLAYFIELD           (NB_BLOCK_X*BLOCK_SIZE) /* Wide of the playfield */

#define WINDOW_WIDTH        (LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + LATERAL_PANEL)
#define WINDOW_HEIGHT       ((NB_BLOCK_Y - FIRST_LINE)*BLOCK_SIZE)

#define START_POS           WINDOW_WIDTH/2.4

#define INTERLINE           90

enum
{
    MENU_PLAY = 0, MENU_CONTROLS = 1, MENU_CREDITS = 2, MENU_OUT = 3
};

#endif /* CONSTANTS_H_INCLUDED */
