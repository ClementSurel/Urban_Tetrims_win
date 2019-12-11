#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "constants.h"
#include "linked_list.h"
#include "game.h"
#include "animation.h"

Uint8 initSprites (Sprites *sprites, SDL_Surface *screen)
{
    /* Variables */
    const SDL_Color black = {0, 0, 0};
    const SDL_Color white = {255, 255, 255};
    const Uint32 lightgrey = SDL_MapRGB (screen->format, 192, 192, 192);
    SDL_Rect position;
    int i, j;

    /* initializes all pointers with NULL */
    sprites->texture = NULL;
    sprites->bg_left = NULL;
    sprites->bg_right = NULL;
    sprites->font = NULL;
    sprites->txt_next = NULL;
    sprites->txt_score = NULL;
    sprites->score = NULL;
    sprites->txt_lvl = NULL;
    sprites->lvl = NULL;
    sprites->txt_nbLines = NULL;
    sprites->nbLines = NULL;

    /* Loads the wall texture */
    sprites->texture = IMG_Load("Sprites/wall.png");
    if (sprites->texture == NULL)
    {
        fprintf(stdout, "Texture sprites has not been successfully loaded\n");
    }

    /* In order to create the opening animation, the background is divided in a left part and a right part.
        Each part is filled with the wall texture.
        The left part has a border on its right and the right part has a border on its left */
    sprites->bg_left = SDL_CreateRGBSurface(SDL_HWSURFACE, WINDOW_WIDTH/2+BORDER, screen->h, 32, 0, 0, 0, 0);
    sprites->bg_right = SDL_CreateRGBSurface(SDL_HWSURFACE, WINDOW_WIDTH/2+BORDER, screen->h, 32, 0, 0, 0, 0);
    SDL_FillRect (sprites->bg_left, NULL, lightgrey);
    SDL_FillRect (sprites->bg_right, NULL, lightgrey);
    for (j = 0; j*sprites->texture->h < screen->h; j++)
    {
        position.y = j*sprites->texture->h;
        for (i = 0; i*sprites->texture->w < WINDOW_WIDTH/2; i++)
        {
            position.x = sprites->bg_left->w - BORDER - (i+1)*sprites->texture->w;
            SDL_BlitSurface (sprites->texture, NULL, sprites->bg_left, &position);
            position.x = BORDER + i*sprites->texture->w;
            SDL_BlitSurface (sprites->texture, NULL, sprites->bg_right, &position);
        }
    }

    /* Load the main font */
    sprites->main_font = TTF_OpenFont("Fonts/painting_with_chocolate.ttf", 80);
    if (sprites->main_font == NULL)
    {
        fprintf(stdout, "An error occurred during memory allocation for the main font\n");
        return 0;
    }

    /* Load the panel texts of the interface */
    sprites->font = TTF_OpenFont("Fonts/high_school_usa_sans.ttf", 22);
    if (sprites->font == NULL)
    {
        fprintf(stdout, "An error occurred during memory allocation for the font\n");
        return 0;
    }

    sprites->txt_score = TTF_RenderText_Shaded (sprites->font, "SCORE", black, white);
    sprites->txt_lvl = TTF_RenderText_Shaded (sprites->font, "LEVEL", black, white);
    sprites->txt_next = TTF_RenderText_Shaded(sprites->font, "NEXT", black, white);
    sprites->txt_nbLines = TTF_RenderText_Shaded (sprites->font, "LINES", black, white);

    return 1;
}

void freeSprites (Sprites *sprites)
{
    SDL_FreeSurface (sprites->texture);
    SDL_FreeSurface (sprites->bg_left);
    SDL_FreeSurface (sprites->bg_right);
    TTF_CloseFont (sprites->main_font);
    TTF_CloseFont (sprites->font);
    SDL_FreeSurface (sprites->txt_next);
    SDL_FreeSurface (sprites->txt_score);
    SDL_FreeSurface (sprites->txt_lvl);
    SDL_FreeSurface (sprites->txt_nbLines);
}

void anim_opening (SDL_Surface *screen, Sprites *sprites)
{
    /* Variables */
    SDL_Rect part, position;
    int delay = 0; /* At the end of the animation, its value will be half the wide of the playfield plus wide of a border */
    const Uint32 lightgrey = SDL_MapRGB(screen->format, 192, 192, 192);
    const Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255);
    const Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);

    /* Blit both background panels */
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface (sprites->bg_left, NULL, screen, &position);
    part.x = BORDER;
    part.y = 0;
    part.w = sprites->bg_right->w;
    part.h = sprites->bg_right->h;
    position.x = WINDOW_WIDTH/2;
    SDL_BlitSurface (sprites->bg_right, &part, screen, &position);

    SDL_Flip(screen);
    SDL_Delay (300);

    for (delay = 0; delay <= (PLAYFIELD+2*BORDER)/2; delay += (PLAYFIELD+2*BORDER)/10)
    {
        /* Fill the screen with white color */
        SDL_FillRect (screen, NULL, white);
        /* Blit both background panels */
        part.x = delay;
        SDL_BlitSurface (sprites->bg_left, &part, screen, NULL);
        if (delay < BORDER)
            part.x = delay;
        else
            part.x = 0;
        position.x = WINDOW_WIDTH/2 + delay - (BORDER-part.x);
        SDL_BlitSurface (sprites->bg_right, &part, screen, &position);

        SDL_Flip (screen);
        SDL_Delay(41);
    }

    /* Sets up the left panel */
    /* Sets up all the borders of the different panels */
    part.w = BORDER + 4*BLOCK_SIZE + BORDER;
    part.h = BORDER + BLOCK_SIZE + BORDER;
    part.x = BLOCK_SIZE;
    part.y = TXT_SCORE;
    SDL_FillRect (screen, &part, lightgrey);
    part.y = SCORE;
    SDL_FillRect (screen, &part, lightgrey);
    part.y = TXT_LVL;
    SDL_FillRect (screen, &part, lightgrey);
    part.y = LVL;
    SDL_FillRect (screen, &part, lightgrey);
    part.y = TXT_NB_LINES;
    SDL_FillRect (screen, &part, lightgrey);
    part.y = NB_LINES;
    SDL_FillRect (screen, &part, lightgrey);
    part.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + BLOCK_SIZE;
    part.y = TXT_NEXT;
    SDL_FillRect (screen, &part, lightgrey);
    /* Sets up the empty panels */
    part.w = 4*BLOCK_SIZE;
    part.h = BLOCK_SIZE;
    part.x = BLOCK_SIZE + BORDER;
    part.y = TXT_SCORE + BORDER;
    SDL_FillRect (screen, &part, white);
    part.y = SCORE + BORDER;
    SDL_FillRect (screen, &part, black);
    part.y = TXT_LVL + BORDER;
    SDL_FillRect (screen, &part, white);
    part.y = LVL + BORDER;
    SDL_FillRect (screen, &part, black);
    part.y = TXT_NB_LINES + BORDER;
    SDL_FillRect (screen, &part, white);
    part.y = NB_LINES + BORDER;
    SDL_FillRect (screen, &part, black);
    part.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + BLOCK_SIZE + BORDER;
    part.y = TXT_NEXT + BORDER;
    SDL_FillRect (screen, &part, white);
    /*Sets up the texts */
    position.x = LATERAL_PANEL/2 - sprites->txt_score->w/2;
    position.y = TXT_SCORE + BORDER;
    SDL_BlitSurface (sprites->txt_score, NULL, screen, &position);
    position.x = LATERAL_PANEL/2 - sprites->txt_lvl->w/2;
    position.y = TXT_LVL + BORDER;
    SDL_BlitSurface (sprites->txt_lvl, NULL, screen, &position);
    position.x = LATERAL_PANEL/2 - sprites->txt_nbLines->w/2;
    position.y = TXT_NB_LINES + BORDER;
    SDL_BlitSurface (sprites->txt_nbLines, NULL, screen, &position);
    position.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + (LATERAL_PANEL)/2 - sprites->txt_next->w/2;
    position.y = TXT_NEXT + BORDER;
    SDL_BlitSurface (sprites->txt_next, NULL, screen, &position);
    /* Draw the case "next tetrim" */
    /* Draw the borders of the case */
    part.w = BORDER + 4*BLOCK_SIZE + BORDER;
    part.h = BORDER + 4*BLOCK_SIZE + BORDER;
    part.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + 1*BLOCK_SIZE;
    part.y = NEXT_CASE;
    SDL_FillRect (screen, &part, lightgrey);
    part.w = 4*BLOCK_SIZE;
    part.h = 4*BLOCK_SIZE;
    part.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + 1*BLOCK_SIZE + BORDER;
    part.y = NEXT_CASE + BORDER;
    SDL_FillRect (screen, &part, black);

}

Uint32 clearCompleteLines (SDL_Surface *screen, Sprites *sprites, GameElements* gameElm)
{
    int i, j, k;
    int completeLine[NB_BLOCK_Y] = {0}; /* Array of booleans : 1 if the line is complete, 0 otherwise */
    Uint32 nbCompleteLines = 0;
    Uint32 actualTime, lastTime; /* time info */

    /* Reads the playfield starting from the bottom to find the complete lines
        and clean them */
    for (j = NB_BLOCK_Y-1; j >= 0; j--)
    {
        /* Check whether the line is complete */
        completeLine[j] = 1;
        for (i = 0; i < NB_BLOCK_X; i++)
        {
            if(gameElm->gMap[i][j] == BLOCK_VOID || gameElm->gMap[i][j] == BLOCK_ACTIVE)
                completeLine[j] = 0;
        }
        /* If the line is complete, erase it */
        if (completeLine[j])
        {
            nbCompleteLines++;
            for (i = 0; i < NB_BLOCK_X; i++)
            {
                gameElm->gMap[i][j] = BLOCK_VOID;
            }
        }

    }

    updateScreen(screen, sprites, gameElm);
    actualTime = SDL_GetTicks();
    lastTime = actualTime;
    while (actualTime - lastTime < 100)
    {
        actualTime = SDL_GetTicks();
    }

    /* Replaces each complete line by the line above */
    for (j = NB_BLOCK_Y-1, k = 0; j >= 0; j--)
    {
        /* If the line j is complete, finds among the lines above the first incomplete line j-k */
        if (completeLine[j-k])
        {
            k++;
            while (completeLine[j-k])
            {
                k++;
            }
        }
        /* If a complete line has been found, replaces the line j by the line j-k */
        if (k >= 1 && j-k >= 0)
        {
            for (i = 0; i < NB_BLOCK_X; i++)
            {
                gameElm->gMap[i][j] = gameElm->gMap[i][j-k];
            }
        }
        /* If j-k is out of the playfield, just erases the complete line */
        else if (k >= 1 && j-k < 0)
        {
            for (i = 0; i < NB_BLOCK_X; i++)
            {
                gameElm->gMap[i][j] = BLOCK_VOID;
            }
        }
    }

    updateScreen (screen, sprites, gameElm);

    return nbCompleteLines;
}

void updateScreen (SDL_Surface *screen, Sprites *sprites, GameElements *gameElm)
{
    /* Variables */
    int i, j;
    Uint32 tetrimColor = 0;
    SDL_Rect part;
    SDL_Rect position;
    double t = 0.000, bleachFactor = 0.000; /* Used to make the active tetrimino blink */
    char data[11];

    /* Color definition */
    /* Static colors */
    static const SDL_Color txt_white = {255, 255, 255};
    static const SDL_Color txt_black = {0, 0, 0};
    static const Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255);
    static const Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);
    static const Uint32 grey = SDL_MapRGB(screen->format, 128, 128, 128);
    static const Uint32 yellow = SDL_MapRGB (screen->format, 231, 231, 24);
    static const Uint32 red = SDL_MapRGB (screen->format, 215, 20, 20);
    static const Uint32 green = SDL_MapRGB (screen->format, 24, 128, 24);
    static const Uint32 orange = SDL_MapRGB (screen->format, 231, 128, 24);
    static const Uint32 purple = SDL_MapRGB (screen->format, 128, 64, 128);
    static const Uint32 blue = SDL_MapRGB (screen->format, 32, 32, 160);
    static const Uint32 cyan = SDL_MapRGB (screen->format, 40, 224, 166);

    /* The color of the active tetrimino changes every time to make it blink */
    /* The following code defines a bleach factor between 0.15 and 0.75 so the color of the active tetromino
       is bleached from 25% to 75% then from 75% to 25% */
    t = SDL_GetTicks()%1000;
    if (t < 500)
        bleachFactor = (t+250)/1000;
    else
        bleachFactor = (1250-t)/1000;
    switch (gameElm->actualTetrim)
    {
    case TETRIM_O:
        tetrimColor = SDL_MapRGB (screen->format, 255-(24*bleachFactor), 255-(24*bleachFactor), 255-(231*bleachFactor));
        break;
    case TETRIM_Z:
        tetrimColor = SDL_MapRGB (screen->format, 255-(20*bleachFactor), 255-(235*bleachFactor), 255-(235*bleachFactor));
        break;
    case TETRIM_I:
        tetrimColor = SDL_MapRGB (screen->format, 255-(215*bleachFactor), 255-(31*bleachFactor), 255-(89*bleachFactor));
        break;
    case TETRIM_S:
        tetrimColor = SDL_MapRGB (screen->format, 255-(231*bleachFactor), 255-(128*bleachFactor), 255-(231*bleachFactor));
        break;
    case TETRIM_J:
        tetrimColor = SDL_MapRGB (screen->format, 255-(223*bleachFactor), 255-(223*bleachFactor), 255-(95*bleachFactor));
        break;
    case TETRIM_T:
        tetrimColor = SDL_MapRGB (screen->format, 255-(128*bleachFactor), 255-(191*bleachFactor), 255-(128*bleachFactor));
        break;
    case TETRIM_L:
        tetrimColor = SDL_MapRGB (screen->format, 255-(24*bleachFactor), 255-(128*bleachFactor), 255-(231*bleachFactor));
        break;
    default:
        break;
    }

        /* Updates the left panel */
        /* Updates the game data */
    part.x = BLOCK_SIZE + BORDER;
    part.w = 4*BLOCK_SIZE;
    part.h = BLOCK_SIZE;
        /* Updates the score */
    part.y = SCORE + BORDER;
    SDL_FillRect (screen, &part, black);
    sprintf(data, "%d", gameElm->score);
    sprites->score = TTF_RenderText_Shaded (sprites->font, data, txt_white, txt_black);
    position.x = LATERAL_PANEL/2 - sprites->score->w/2;
    position.y = SCORE + BORDER;
    SDL_BlitSurface (sprites->score, NULL, screen, &position);
        /* Updates the level */
    part.y = LVL + BORDER;
    SDL_FillRect (screen, &part, black);
    sprintf(data, "%d", gameElm->level);
    sprites->lvl = TTF_RenderText_Shaded (sprites->font, data, txt_white, txt_black);
    position.x = LATERAL_PANEL/2 - sprites->lvl->w/2;
    position.y = LVL + BORDER;
    SDL_BlitSurface (sprites->lvl, NULL, screen, &position);
        /* Updates the number of complete lines */
    part.y = NB_LINES + BORDER;
    SDL_FillRect (screen, &part, black);
    sprintf(data, "%d", gameElm->nbCompleteLines);
    sprites->nbLines = TTF_RenderText_Shaded (sprites->font, data, txt_white, txt_black);
    position.x = LATERAL_PANEL/2 - sprites->nbLines->w/2;
    position.y = NB_LINES + BORDER;
    SDL_BlitSurface (sprites->nbLines, NULL, screen, &position);

        /* Updates the playfield */
    for (j = FIRST_LINE; j < NB_BLOCK_Y; j++)
    {
        for (i = 0; i < NB_BLOCK_X; i++)
        {
            part.x = LATERAL_PANEL + BORDER + i*BLOCK_SIZE + GRID_WIDE;
            part.w = BLOCK_SIZE - 2*GRID_WIDE;
            part.h = BLOCK_SIZE - 2*GRID_WIDE;
            part.y = j*BLOCK_SIZE + GRID_WIDE - (FIRST_LINE*BLOCK_SIZE);
            switch (gameElm->gMap[i][j])
            {
                case BLOCK_VOID:
                    SDL_FillRect (screen, &part, white);
                    break;
                case BLOCK_CYAN:
                    SDL_FillRect (screen, &part, cyan);
                    break;
                case BLOCK_GREEN:
                    SDL_FillRect (screen, &part, green);
                    break;
                case BLOCK_ORANGE:
                    SDL_FillRect (screen, &part, orange);
                    break;
                case BLOCK_PURPLE:
                    SDL_FillRect (screen, &part, purple);
                    break;
                case BLOCK_BLUE:
                    SDL_FillRect (screen, &part, blue);
                    break;
                case BLOCK_YELLOW:
                    SDL_FillRect (screen, &part, yellow);
                    break;
                case BLOCK_RED:
                    SDL_FillRect (screen, &part, red);
                    break;
                case BLOCK_ACTIVE:
                    SDL_FillRect (screen, &part, tetrimColor);
                    break;
            }
        }
    }

        /* Sets up the right pannel */
        /* Fills the case "next" with the tetrimino */
    part.w = BLOCK_SIZE - 2*GRID_WIDE;
    part.h = BLOCK_SIZE - 2*GRID_WIDE;
    for (j = 0; j < 4; j++)
    {
        part.y = NEXT_CASE + BORDER + j*BLOCK_SIZE + GRID_WIDE;
        for (i = 0; i < 4; i++)
        {
            part.x = LATERAL_PANEL + BORDER + PLAYFIELD + BORDER + BLOCK_SIZE + BORDER + i*BLOCK_SIZE + GRID_WIDE;
            switch (gameElm->nextTetrimMap[i][j])
            {
                case BLOCK_VOID:
                    SDL_FillRect (screen, &part, black);
                    break;
                case BLOCK_ACTIVE:
                    switch (gameElm->nextTetrim)
                    {
                    case TETRIM_O:
                        SDL_FillRect (screen, &part, yellow);
                        break;
                    case TETRIM_Z:
                        SDL_FillRect (screen, &part, red);
                        break;
                    case TETRIM_I:
                        SDL_FillRect (screen, &part, cyan);
                        break;
                    case TETRIM_S:
                        SDL_FillRect (screen, &part, green);
                        break;
                    case TETRIM_J:
                        SDL_FillRect (screen, &part, blue);
                        break;
                    case TETRIM_T:
                        SDL_FillRect (screen, &part, purple);
                        break;
                    case TETRIM_L:
                        SDL_FillRect (screen, &part, orange);
                        break;
                    default:
                        SDL_FillRect (screen, &part, grey);
                        break;
                    }
                    break;
            }
        }
    }

    SDL_Flip (screen);

    SDL_FreeSurface (sprites->score);
    SDL_FreeSurface (sprites->lvl);
    SDL_FreeSurface (sprites->nbLines);

}

int menuControls (SDL_Surface *screen, SDL_Surface *background)
{
    int continueProg = 1, continueControls = 1; /* Booleans */
    SDL_Event event;
    SDL_Surface *controls_bg = NULL, *external_rect = NULL, *internal_rect = NULL;
    SDL_Surface *keyboard = NULL;
    TTF_Font *street36 = TTF_OpenFont ("Fonts/high_school_usa_sans.ttf", 36);
    TTF_Font *street18 = TTF_OpenFont ("Fonts/high_school_usa_sans.ttf", 18);
    SDL_Color txt_white = {255, 255, 255};
    SDL_Surface *controls = NULL, *text = NULL;
    SDL_Rect part, position;
    int i;

    /* Load the keyboard image */
    keyboard = IMG_Load("Sprites/keyboard.png");

    /* Set up the background using the title screen's background as a model*/
    controls_bg = SDL_CreateRGBSurface (SDL_HWSURFACE, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
    SDL_BlitSurface (background, NULL, controls_bg, NULL);

    external_rect = SDL_CreateRGBSurface (SDL_HWSURFACE, WINDOW_WIDTH-2*BLOCK_SIZE, WINDOW_HEIGHT-2*BLOCK_SIZE, 32, 0, 0, 0, 0);
    SDL_FillRect ( external_rect, NULL, SDL_MapRGB(external_rect->format, 192, 192, 192) );
    position.x = BLOCK_SIZE;
    position.y = BLOCK_SIZE;
    SDL_BlitSurface (external_rect, NULL, controls_bg, &position);

    internal_rect = SDL_CreateRGBSurface (SDL_HWSURFACE, external_rect->w-2*BORDER, external_rect->h-2*BORDER, 32, 0, 0, 0, 0);
    SDL_FillRect ( internal_rect, NULL, SDL_MapRGB(internal_rect->format, 0, 0, 0) );
    position.x += BORDER;
    position.y += BORDER;
    SDL_BlitSurface (internal_rect, NULL, controls_bg, &position);

    /* Prints controls screen title */
    controls = TTF_RenderText_Blended (street36, "CONTROLS", txt_white);
    position.x += internal_rect->w/2 - controls->w/2;
    position.y += BLOCK_SIZE;
    SDL_BlitSurface (controls, NULL, controls_bg, &position);

    /* Print the controls */
    position.x = BLOCK_SIZE+BORDER;
    position.y += controls->h;
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            part.x = 565;
            part.y = 168;
            text = TTF_RenderText_Blended (street18, "Rotate the tetrim", txt_white);
            break;
        case 1:
            part.x = 533;
            part.y = 201;
            text = TTF_RenderText_Blended (street18, "Move the tetrim to the left", txt_white);
            break;
        case 2:
            part.x = 599;
            part.y = 201;
            text = TTF_RenderText_Blended (street18, "Move the tetrim to the right", txt_white);
            break;
        case 3:
            part.x = 565;
            part.y = 201;
            text = TTF_RenderText_Blended (street18, "Hard drop activation : make the tetrim fall faster and earn points", txt_white);
            break;
        }
        part.w = 34;
        part.h = 34;
        position.x = 2*BLOCK_SIZE + BORDER;
        position.y += 2*BLOCK_SIZE;
        SDL_BlitSurface (keyboard, &part, controls_bg, &position);
        position.x += part.w + BLOCK_SIZE/2;
        position.y += part.h/2 - text->h/2;
        SDL_BlitSurface (text, NULL, controls_bg, &position);
        SDL_FreeSurface (text);
    }

    /* Update the screen */
    SDL_BlitSurface (controls_bg, NULL, screen, NULL);
    SDL_Flip(screen);

    /* Wait for a reaction from the player */
    while (continueProg && continueControls)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            continueProg = 0;
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_ESCAPE
                                            || event.key.keysym.sym == SDLK_SPACE
                                            || event.key.keysym.sym == SDLK_RETURN))
            continueControls = 0;
    }

    /* Free the images and the font */
    TTF_CloseFont (street18);
    TTF_CloseFont (street36);
    SDL_FreeSurface (keyboard);
    SDL_FreeSurface (controls);
    SDL_FreeSurface (internal_rect);
    SDL_FreeSurface (external_rect);
    SDL_FreeSurface (controls_bg);

    return continueProg;
}

int pause (SDL_Surface *screen)
{
    int continueProg = 1;
    SDL_Event event;
    SDL_Surface *background = NULL, *pause = NULL;
    TTF_Font *font = NULL;
    SDL_Color black = {0, 0, 0};
    SDL_Rect position = {0};

    /* Make a copy of the actual screen */
    background = SDL_CreateRGBSurface(SDL_HWSURFACE, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
    SDL_BlitSurface (screen, NULL, background, NULL);

    /* Print pause text */
    font = TTF_OpenFont ("Fonts/high_school_usa_sans.ttf", 60);
    pause = TTF_RenderText_Blended (font, "PAUSE", black);
    position.x = WINDOW_WIDTH/2 - pause->w/2;
    position.y = WINDOW_HEIGHT/2 - pause->h/2;
    SDL_BlitSurface (pause, NULL, screen, &position);
    SDL_Flip(screen);

    /* Wait for a reaction from the player */
    while (event.type != SDL_KEYDOWN && event.type != SDL_QUIT)
    {
        SDL_WaitEvent (&event);
    }
    if (event.type == SDL_QUIT)
        continueProg = 0;

    /* Refresh the screen */
    SDL_BlitSurface (background, NULL, screen, NULL);
    SDL_Flip(screen);

    /* Free Surfaces and font */
    TTF_CloseFont (font);
    SDL_FreeSurface (pause);
    SDL_FreeSurface (screen);

    return continueProg;
}

