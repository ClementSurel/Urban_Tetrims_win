/*********************************************/
/** Manage the game pictures and animations **/
/*********************************************/

#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED


#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "constants.h"
#include "linked_list.h"
#include "game.h"

/* y position for the different elements of the lateral panel from the top to the bottom */
#define TXT_NEXT        BLOCK_SIZE
#define NEXT_CASE       TXT_NEXT + BORDER + 1*BLOCK_SIZE + BORDER + BLOCK_SIZE/4
#define TXT_SCORE       NEXT_CASE + BORDER + 4*BLOCK_SIZE + BORDER + 88
#define SCORE           TXT_SCORE + BORDER + BLOCK_SIZE + BORDER + 7
#define TXT_LVL         SCORE + BORDER + BLOCK_SIZE + BORDER + 7
#define LVL             TXT_LVL + BORDER + BLOCK_SIZE + BORDER + 7
#define TXT_NB_LINES    LVL + BORDER + BLOCK_SIZE + BORDER + 7
#define NB_LINES        TXT_NB_LINES + BORDER + BLOCK_SIZE + 7


typedef struct Sprites /* Contains all the game sprites and the font used for the interface */
{
    SDL_Surface* bg_left;
    SDL_Surface* bg_right;
    SDL_Surface *texture;
    TTF_Font *main_font;
    TTF_Font *font;
    SDL_Surface *txt_next;
    SDL_Surface *txt_score;
    SDL_Surface *score;
    SDL_Surface *txt_lvl;
    SDL_Surface *lvl;
    SDL_Surface *txt_nbLines;
    SDL_Surface *nbLines;
} Sprites;


/** Initializes the structure Sprites.
    Takes SDL_Surface *screen in param since it needs the information about the pixel format **/
Uint8 initSprites (Sprites*, SDL_Surface *screen);

/** Frees the structure Sprites. **/
void freeSprites (Sprites*);

/** Manages the opening animation **/
void anim_opening(SDL_Surface* screen, Sprites*);

/** Clears the completed lines with a quick animation
    Returns the number of lines that has been cleared **/
Uint32 clearCompleteLines (SDL_Surface *screen, Sprites*, GameElements*);

/** Generates the pictures for the screen **/
void updateScreen (SDL_Surface *screen, Sprites*, GameElements*);

int menuControls (SDL_Surface *screen, SDL_Surface *background);

int pause (SDL_Surface*);

#endif // ANIMATION_H_INCLUDED
