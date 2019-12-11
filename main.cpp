/**
 *
 *  This source code use the SDL library version 1.2 with the extensions SDL_image and SDL_ttf
 *
 *  The source code is composed of 4 header and 4 source code files:
 *  constants.h
 *  main.cpp
 *  game.h
 *  game.cpp
 *  animation.h
 *  animation.cpp
 *  linked_chain.h
 *  linked_chain.cpp
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "constants.h"
#include "game.h"
#include "animation.h"

int main ( int argc, char** argv )
{
    /* Variables */
    SDL_Surface *screen = NULL, *title = NULL;
    SDL_Surface *start = NULL, *controls = NULL, *credits = NULL;
    SDL_Surface *background = NULL, *cache = NULL;
    double t, alpha;
    SDL_Color blue = {70, 140, 210};
    SDL_Rect part, cache_part, position;
    SDL_Event event;
    int continueProg = 1; // Boolean
    int player_choice = MENU_PLAY;
    Sprites sprites;

    /* SDL initialization */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        fprintf(stderr, "Error during SDL initialization : %s\n", SDL_GetError() );
        exit (EXIT_FAILURE);
    }

    /* SDL_ttf initialization */
    if ( TTF_Init() < 0)
    {
        fprintf(stderr, "An error occurred during SDL_ttf initialization\n");
        exit(EXIT_FAILURE);
    }

    /* Open the windows */
    SDL_Surface *icon = IMG_Load("Sprites/wall.png");
    SDL_WM_SetIcon (icon, NULL);
    SDL_WM_SetCaption ("Urban Tetrims", NULL);
    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        fprintf(stderr, "Impossible to open the window\n");
        exit(EXIT_FAILURE);
    }

    /* Load the sprites */
    if (!initSprites (&sprites, screen))
    {
        fprintf(stderr, "An error occurred during sprites loading\n");
        exit(EXIT_FAILURE);
    }

    /* Print the background */
    background = SDL_CreateRGBSurface (SDL_HWSURFACE, screen->w, screen->h, 32, 0, 0, 0, 0);
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface (sprites.bg_left, NULL, background, &position);
    part.x = BORDER;
    part.y = 0;
    part.w = sprites.bg_right->w-BORDER;
    part.h = sprites.bg_right->h;
    position.x = WINDOW_WIDTH/2;
    position.y = 0;
    SDL_BlitSurface (sprites.bg_right, &part, background, &position);
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface (background, NULL, screen, &position);

    /* Write the menu's strings */
    /* title */
    title = TTF_RenderText_Blended (sprites.main_font, "URBAN TETRIMS", blue);
    position.x = screen->w/2 - title->w/2;
    position.y = screen->h/4 - title->h/2;
    SDL_BlitSurface(title, NULL, screen, &position);
    /* start menu */
    start = TTF_RenderText_Blended (sprites.main_font, "START", blue);
    position.x = screen->w/2 - start->w/2;
    position.y = START_POS;
    SDL_BlitSurface(start, NULL, screen, &position);
    /* controls menu */
    controls = TTF_RenderText_Blended (sprites.main_font, "CONTROLS", blue);
    position.x = screen->w/2 - controls->w/2;
    position.y = START_POS + INTERLINE;
    SDL_BlitSurface(controls, NULL, screen, &position);
    /* credits menu */
    credits = TTF_RenderText_Blended (sprites.main_font, "CREDITS", blue);
    position.x = screen->w/2 - credits->w/2;
    position.y = START_POS + 2*INTERLINE;
    SDL_BlitSurface(credits, NULL, screen, &position);

    /* Prepares the cache */
    cache = SDL_CreateRGBSurface (SDL_HWSURFACE, screen->w, screen->h, 32, 0, 0, 0, 0);
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface (background, NULL, cache, &position);

    /* The color of the active tetrimino changes every time to make it blink */
    /* The following code defines a bleach factor between 0.15 and 0.75 so the color of the active tetromino
       is bleached from 25% to 75% then from 75% to 25% */
    t = SDL_GetTicks() % 700;
    if (t < 350)
        alpha = 255*(t/350);
    else
        alpha = 255*((700-t)/350);
    SDL_SetAlpha (cache, SDL_SRCALPHA, alpha);
    cache_part.x = screen->w/2 - controls->w/2;
    cache_part.y = START_POS;
    cache_part.w = controls->w;
    cache_part.h = controls->h;
    SDL_BlitSurface (cache, &cache_part, screen, &cache_part);

    SDL_Flip(screen);

    /* Main Loop */
    while (continueProg)
    {
        if ( SDL_PollEvent (&event) )
        switch (event.type)
        {
            case SDL_QUIT:
                continueProg = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    continueProg = 0;
                    break;
                case SDLK_SPACE:
                    switch (player_choice)
                    {
                    case MENU_PLAY:
                        continueProg = playGame (screen, &sprites);
                        break;
                    case MENU_CONTROLS:
                        continueProg = menuControls (screen, background);
                        break;
                    }
                    break;
                case SDLK_DOWN:
                    if (player_choice + 1 != MENU_OUT)
                    {
                        player_choice++;
                        cache_part.y += INTERLINE;
                    }
                    break;
                case SDLK_UP:
                    if (player_choice - 1 >= 0)
                    {
                        player_choice--;
                        cache_part.y -= INTERLINE;
                    }
                    break;
                default:
                    break;
                }
        }

        if (continueProg)
        {
            /* Refresh the title screen */
            position.x = 0;
            position.y = 0;
            SDL_BlitSurface (background, NULL, screen, &position);

            position.x = WINDOW_WIDTH/2 - title->w/2;
            position.y = WINDOW_WIDTH/4 - title->h/2;
            SDL_BlitSurface(title, NULL, screen, &position);

            position.x = WINDOW_WIDTH/2 - start->w/2;
            position.y = START_POS;
            SDL_BlitSurface(start, NULL, screen, &position);

            position.x = WINDOW_WIDTH/2 - controls->w/2;
            position.y = START_POS + INTERLINE;
            SDL_BlitSurface(controls, NULL, screen, &position);

            position.x = WINDOW_WIDTH/2 - credits->w/2;
            position.y = START_POS + 2*INTERLINE;
            SDL_BlitSurface(credits, NULL, screen, &position);

            t = SDL_GetTicks() % 700;
            if (t < 350)
                alpha = 255*(t/350);
            else
                alpha = 255*((700-t)/350);
            SDL_SetAlpha (cache, SDL_SRCALPHA, alpha);
            cache_part.x = screen->w/2 - controls->w/2;
            cache_part.w = controls->w;
            cache_part.h = controls->h;
            SDL_BlitSurface (cache, &cache_part, screen, &cache_part);
            SDL_Flip(screen);
        }
    }

    SDL_FreeSurface (background);
    SDL_FreeSurface (cache);
    SDL_FreeSurface (title);
    SDL_FreeSurface (start);
    SDL_FreeSurface (controls);
    SDL_FreeSurface (credits);
    SDL_FreeSurface (icon);

    freeSprites (&sprites);

    TTF_Quit();

    SDL_Quit();

    return EXIT_SUCCESS;
}

