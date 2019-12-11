#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "constants.h"
#include "game.h"
#include "animation.h"
#include "linked_list.h"

Uint8 initGameElements (GameElements *gameElm)
{
    Uint32 i = 0, j = 0;

    for (i = 0; i < NB_BLOCK_X; i++)
    {
        for (j = 0; j < NB_BLOCK_Y; j++)
        {
            gameElm->gMap[i][j] = BLOCK_VOID;
        }
    }

    gameElm->bag = LNK_createBag();
    if (gameElm->bag == NULL)
    {
        return 0;
    }

    LNK_fillBag(gameElm->bag);

        /* Affects actualTetrim sub-variable for safety only.
            This first tetrimino will be ignored */
    gameElm->actualTetrim = TETRIM_I;

        /* Here, the actual first tetrim is drawn */
    gameElm->nextTetrim = LNK_drawTetrim(gameElm->bag);

    gameElm->rotationState = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            gameElm->nextTetrimMap[i][j] = BLOCK_VOID;
        }
    }

    gameElm->dimension = 0;
    gameElm->block1.i = 0;
    gameElm->block1.j = 0;
    gameElm->tetrimActive = 0;
    gameElm->score = 0;
    gameElm->level = 1;
    gameElm->nbCompleteLines = 0;

    return 1;
}

void freeGameElements (GameElements *gameElm)
{
    LNK_freeList (gameElm->bag);
}

Uint8 playGame (SDL_Surface *screen, Sprites *sprites)
{
    /* Variables */
    Uint8 continueProg = 1, continueGame = 1; /* Booleans */
    GameElements gameElm;
    SDL_Event event;
    Uint8 newTetrimGenerated = 0; /* Booleans */
    Uint32 actualTime = 0, lastScreen_time = 0; /* time info */
    Uint32 lastMove_time = 0, lastFall_time = 0, onStack_time = 0; /* time info */
    Uint32 movingPeriod = MOVING_PERIOD_START, falling_period; /* period info */
    Uint32 normalFalling_period = 1000; /* period info */
    Uint8 movingTetrimToLeft = 0, movingTetrimToRight = 0; /* Booleans */
    Uint8 hard_drop = 0, tetrimOnStack = 0; /* Booleans */
    int new_points = 0;
    int nbMovesOnStack = 0;
    int nbLines = 0;
    Direction direction = DIR_LEFT;
    SDL_Surface *gameOver = NULL;
    SDL_Rect position;
    SDL_Color orange = {255, 128, 0};

    /* Initialize game elements */
    if ( ! initGameElements (&gameElm) )
    {
        fprintf(stderr, "An unknow error occurred during the creation of the game elements\n");
        return 0;
    }

    /* Set up "game over" panel */
    gameOver = TTF_RenderText_Blended(sprites->main_font, "GAME OVER", orange);

    /* Update the period info */
    normalFalling_period = 1000;
    falling_period = normalFalling_period;

    /* Set up the different timers */
    actualTime = SDL_GetTicks();
    lastFall_time = actualTime;
    lastMove_time = actualTime;

    /* Trigger the opening animation */
    anim_opening(screen, sprites);

    /* Main loop */
    while (continueGame && continueProg)
    {
        /* if there is no active tetrimino, generates a new one */
        if (!gameElm.tetrimActive)
        {
            newTetrimGenerated = generateNewTetrim (&gameElm);
            putTetrim (gameElm.nextTetrimMap, gameElm.nextTetrim);
            lastFall_time = SDL_GetTicks();
            lastMove_time = lastFall_time;
            updateScreen(screen, sprites, &gameElm);
        }

        /* Manage the events */
        if ( SDL_PollEvent (&event) )
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    continueProg = 0;
                    break;
                case SDL_ACTIVEEVENT:
                    if ( ( (event.active.state & SDL_APPACTIVE) == SDL_APPACTIVE
                        || (event.active.state & SDL_APPINPUTFOCUS) == SDL_APPINPUTFOCUS )
                        && event.active.gain == 0)
                        pause (screen);
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            continueGame = 0;
                            break;
                        case SDLK_p:
                            continueProg = pause(screen);
                            break;
                        case SDLK_UP:
                            tetrimRotates (&gameElm);
                            if (tetrimOnStack)
                            {
                                nbMovesOnStack++;
                                if (nbMovesOnStack < 15)
                                    onStack_time = SDL_GetTicks();
                            }
                            break;
                        case SDLK_LEFT:
                            direction = DIR_LEFT;
                            tetrimMoves (&gameElm, direction);
                            movingPeriod = MOVING_PERIOD_START;
                            movingTetrimToLeft = 1;
                            if (tetrimOnStack)
                            {
                                nbMovesOnStack++;
                                if (nbMovesOnStack < 15)
                                    onStack_time = SDL_GetTicks();
                            }
                            lastMove_time = SDL_GetTicks();
                            break;
                        case SDLK_RIGHT:
                            direction = DIR_RIGHT;
                            tetrimMoves (&gameElm, direction);
                            movingPeriod = MOVING_PERIOD_START;
                            movingTetrimToRight = 1;
                            if (tetrimOnStack)
                            {
                                nbMovesOnStack++;
                                if (nbMovesOnStack < MAX_MOVES_ON_STACK)
                                    onStack_time = SDL_GetTicks();
                            }
                            lastMove_time = SDL_GetTicks();
                            break;
                        case SDLK_DOWN:
                            falling_period = (normalFalling_period < HARD_DROP_PERIOD) ?
                                                            normalFalling_period : HARD_DROP_PERIOD;
                            if (!tetrimOnStack)
                            {
                                tetrimOnStack = tetrimFalls(&gameElm);
                                lastFall_time = SDL_GetTicks();
                                if (tetrimOnStack)
                                    onStack_time = SDL_GetTicks();
                                else
                                {
                                    hard_drop = 1;
                                    new_points = 2;
                                    gameElm.score = ((gameElm.score + new_points) < SCORE_MAX) ?
                                                                (gameElm.score + new_points) : SCORE_MAX;
                                }
                            }
                            else
                            {
                                tetrimOnStack = tetrimFalls(&gameElm);
                                if (! tetrimOnStack)
                                {
                                    nbMovesOnStack = 0;
                                    new_points = 2;
                                    gameElm.score = ((gameElm.score + new_points) < SCORE_MAX) ?
                                                                (gameElm.score + new_points) : SCORE_MAX;
                                }
                                lastFall_time = SDL_GetTicks();
                            }
                            break;
                        default:
                            break;
                    }
                    break; // end case SDLK_KEYDOWN
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_DOWN :
                            falling_period = normalFalling_period;
                            hard_drop = 0;
                            break;
                        case SDLK_LEFT :
                            movingTetrimToLeft = 0;
                            if (movingTetrimToRight)
                                direction = DIR_RIGHT;
                            break;
                        case SDLK_RIGHT :
                            movingTetrimToRight = 0;
                            if (movingTetrimToLeft)
                                direction = DIR_LEFT;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        actualTime = SDL_GetTicks();

        /* If the player keeps direction button down, move the tetrimino */
        if ( gameElm.tetrimActive && (movingTetrimToLeft || movingTetrimToRight)
            && (actualTime - lastMove_time >= movingPeriod) )
        {
            tetrimMoves (&gameElm, direction);
            movingPeriod = MOVING_PERIOD;
            lastMove_time = actualTime;
        }

        /* Make the active tetrim fall */
        if ( gameElm.tetrimActive
        && (actualTime - lastFall_time >= falling_period) )
        {
            if (!tetrimOnStack)
            {
                tetrimOnStack = tetrimFalls (&gameElm);
                if (tetrimOnStack)
                    onStack_time = SDL_GetTicks();
                else if (hard_drop)
                {
                    new_points = 2;
                    gameElm.score = ((gameElm.score + new_points) < SCORE_MAX) ?
                                                        (gameElm.score + new_points) : SCORE_MAX;
                }
                lastFall_time = actualTime;
            }
            else
            {
                tetrimOnStack = tetrimFalls (&gameElm);
                lastFall_time = actualTime;
            }
        }

        /* After 0.5 seconds on the stack, the tetrim is locked (i.e. becomes inactive)
        and the number of complete lines is evaluated */
        if ( tetrimOnStack && (actualTime - onStack_time >= LOCK_DELAY) )
        {
            if ( locksTetrim(&gameElm) ) /* If there is at least one complete line */
            {
                /* Clear the complete lines */
                nbLines = clearCompleteLines (screen, sprites, &gameElm);
                gameElm.nbCompleteLines += nbLines;

                /* Update the score */
                switch (nbLines)
                {
                    case 0:
                        break;
                    case 1:
                        new_points = 100*gameElm.level;
                        break;
                    case 2:
                        new_points += 300*gameElm.level;
                        break;
                    case 3:
                        new_points += 500*gameElm.level;
                        break;
                    case 4:
                        new_points += 800*gameElm.level;
                        break;
                }
                gameElm.score = ( gameElm.score + new_points < SCORE_MAX) ?
                                                    (gameElm.score + new_points) : SCORE_MAX;

                /* Updates the level */
                if (gameElm.nbCompleteLines >= gameElm.level*10)
                {
                    gameElm.level++;
                    normalFalling_period = pow( 0.8 - ((gameElm.level-1)*0.007), gameElm.level-1)*1000;
                    falling_period = (falling_period == HARD_DROP_PERIOD) ?
                                                    HARD_DROP_PERIOD : normalFalling_period;
                }

            } /* end clearing the complete lines */

            tetrimOnStack = 0;
            nbMovesOnStack = 0;
        } /* end locking the tetrim */

        /* Refresh the screen */
        if ( actualTime - lastScreen_time >= 30 )
        {
            updateScreen (screen, sprites, &gameElm);
            lastScreen_time = actualTime;
        }

        /* Prints game over if the generation of a new tetrim failed */
        if (!newTetrimGenerated)
        {
            position.w = 200;
            position.h = 200;
            position.x = screen->w/2 - gameOver->w/2;
            position.y = screen->h/2 - gameOver->h/2;
            SDL_BlitSurface(gameOver, NULL, screen, &position);
            SDL_Flip (screen);

            /* Oblige the player to quit the game or the program */
            while (continueProg && continueGame)
            {
                SDL_WaitEvent (&event);
                if (event.type == SDL_QUIT)
                    continueProg = 0;
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                    continueGame = 0;
            }

        } /* Game over */
    } /* Main Loop */

    freeGameElements (&gameElm);

    SDL_FreeSurface (gameOver);

    return continueProg;
}

Uint8 generateNewTetrim (GameElements *gameElm)
{
    int i, j, k, l;
    int tetrimLeft = 0;
    Uint32 tetrimMap[4][4] = {BLOCK_VOID};
    Uint8 newTetrimGenerated = 0;

    if (gameElm->bag == NULL)
        return 0;

    /* The next tetrimino becomes the new acitve tetrimino */
    gameElm->actualTetrim = gameElm->nextTetrim;

    /* Checks whether there are some tetriminoes left in the bag
       If not, fills the bag */
    tetrimLeft = LNK_getNbElm(gameElm->bag);
    if (!tetrimLeft)
        LNK_fillBag (gameElm->bag);

    /* Draws a next tetrimino from the bag */
    gameElm->nextTetrim = LNK_drawTetrim (gameElm->bag);

    /* Paints the next tetrimino in the tetrimMap */
    putTetrim (tetrimMap, gameElm->actualTetrim);

    /* Put the new active tetrim in the playfield
       Indicates with a boolean whether the tetrimino has been successfully put in the playfield or not */
    newTetrimGenerated = 1;
    for (j = FIRST_LINE-1, l = 0; l < 4; j++, l++)
    {
        for (i = NB_BLOCK_X/2-2, k = 0; k < 4; i++, k++)
        {
            if (tetrimMap[k][l] == BLOCK_ACTIVE && gameElm->gMap[i][j] == BLOCK_VOID)
                gameElm->gMap[i][j] = BLOCK_ACTIVE;
            else if (tetrimMap[k][l] == BLOCK_ACTIVE && gameElm->gMap[i][j] != BLOCK_VOID)
                newTetrimGenerated = 0;
        }
    }

    /* Updates information about the new tetrimino */
    i = NB_BLOCK_X/2;
    switch (gameElm->actualTetrim)
    {
        case TETRIM_I:
            gameElm->block1.i = i-2;
            gameElm->block1.j = FIRST_LINE;
            gameElm->dimension = 4;
            break;
        case TETRIM_O:
            gameElm->block1.i = i-1;
            gameElm->block1.j = FIRST_LINE;
            gameElm->dimension = 2;
            break;
        case TETRIM_T:
        case TETRIM_L:
        case TETRIM_J:
        case TETRIM_Z:
        case TETRIM_S:
            gameElm->block1.i = i-2;
            gameElm->block1.j = FIRST_LINE;
            gameElm->dimension = 3;
    }
    gameElm->tetrimActive = 1;
    gameElm->rotationState = 0;

    return newTetrimGenerated;
}

Uint8 tetrimFalls (GameElements *gameElm)
{
    Uint8 activeBlock = 0, canFall = 1;
    int i, j;

    /* Checks whether there is an active tetrimino and whether it can fall lower or not */
    for (j = 0; j < NB_BLOCK_Y; j++)
    {
        for (i = 0; i < NB_BLOCK_X; i++)
        {
            if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
            {
                activeBlock = 1;
                if (j+1 < NB_BLOCK_Y)
                {
                    if (gameElm->gMap[i][j+1] != BLOCK_ACTIVE
                        && gameElm->gMap[i][j+1] != BLOCK_VOID)
                        canFall = 0;
                }
                else
                    canFall = 0;
            }
        }
    }

    /* If the active tetrim can fall, makes all the active bloc fall one case beneath */
    if (activeBlock && canFall)
    {
        for (j = NB_BLOCK_Y-1; j >= 0; j--)
        {
            for (i = 0; i < NB_BLOCK_X; i++)
            {
                if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                {
                    gameElm->gMap[i][j] = BLOCK_VOID;
                    gameElm->gMap[i][j+1] = BLOCK_ACTIVE;
                }
            }
        }
        gameElm->block1.j++;
    }
    /* Else if there is an active tetrim but this last cannot fall, return 1 to indicate
        that the active tetrim has touched the ground or the stack */
    else if (activeBlock && !canFall)
        return 1;

    return 0;
}

void tetrimMoves (GameElements *gameElm, Direction dir)
{
    int i = 0, j = 0;
    Uint8 canMove = 1;

    if (gameElm->tetrimActive)
    {
        /* Check whether the active tetrim can move */
        if (dir == DIR_LEFT)
        {
            for (j = 0; j < NB_BLOCK_Y; j++)
            {
                for (i = 0; i < NB_BLOCK_X; i++)
                {
                    if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                    {
                        if (i-1 < 0)
                            canMove = 0;
                        else if (gameElm->gMap[i-1][j] != BLOCK_VOID && gameElm->gMap[i-1][j] != BLOCK_ACTIVE)
                            canMove = 0;
                    }
                }
            }
            /* if the tetrim can move, all the active blocks are moved */
            if (canMove)
            {
                for (i = 1; i < NB_BLOCK_X; i++)
                {
                    for (j = 0; j < NB_BLOCK_Y; j++)
                    {
                        if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                        {
                            gameElm->gMap[i][j] = BLOCK_VOID;
                            gameElm->gMap[i-1][j] = BLOCK_ACTIVE;
                        }
                    }
                }
                gameElm->block1.i--;
            }
        }

        else if (dir == DIR_RIGHT)
        {
            for (j = 0; j < NB_BLOCK_Y; j++)
            {
                for (i = 0; i < NB_BLOCK_X; i++)
                {
                    if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                    {
                        if (i+1 >= NB_BLOCK_X)
                            canMove = 0;
                        else if (gameElm->gMap[i+1][j] != BLOCK_VOID && gameElm->gMap[i+1][j] != BLOCK_ACTIVE)
                            canMove = 0;
                    }
                }
            }
            /* if the tetrim can move, all the active blocks are moved */
            if (canMove)
            {
                for (i = NB_BLOCK_X-2; i >= 0; i--)
                {
                    for (j = 0; j < NB_BLOCK_Y; j++)
                    {
                        if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                        {
                            gameElm->gMap[i][j] = BLOCK_VOID;
                            gameElm->gMap[i+1][j] = BLOCK_ACTIVE;
                        }
                    }
                }
                gameElm->block1.i++;
            }
        }
    }
}

void tetrimRotates (GameElements *gameElm)
{
    int i = 0, j = 0, a = 0, b = 0;
    int i_start = 0, j_start = 0;
    Uint8 canTurn = 1, nTest = 1;
    Uint32 **tetrimMap = NULL;

    /* Check whether the tetrim is active */
    if (!gameElm->tetrimActive)
        return;

    /* Copy the tetrim map and turns it in a clockwise way */
    tetrimMap = (Uint32**)malloc(sizeof(Uint32*)*gameElm->dimension);
    for (i = 0; i < gameElm->dimension; i++)
    {
        tetrimMap[i] = (Uint32*)malloc(sizeof(Uint32)*gameElm->dimension);
    }

    for (j = 0, a = gameElm->dimension-1; j < gameElm->dimension && a >= 0; j++, a--)
    {
        for (i = 0, b = 0; i < gameElm->dimension && b < gameElm->dimension; i++, b++)
        {
            if (gameElm->block1.i+i < 0 || gameElm->block1.i+i >= NB_BLOCK_X
                || gameElm->block1.j+j >= NB_BLOCK_Y)
                tetrimMap[a][b] = BLOCK_VOID;
            else
                tetrimMap[a][b] = gameElm->gMap[gameElm->block1.i+i][gameElm->block1.j+j];
        }
    }

    /* Take the 5 tests to know whether the tetrimino copy can be put inside the gMap without creating any collision */
    nTest = 1;
    do
    {
        switch (gameElm->actualTetrim)
        {
            case TETRIM_I:
                switch (gameElm->rotationState)
                {
                    case 0:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i-2;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j;
                                break;
                            case 4:
                                i_start = gameElm->block1.i-2;
                                j_start = gameElm->block1.j+1;
                                break;
                            case 5:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j-2;
                                break;
                        }
                    break;
                case 1:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i+2;
                                j_start = gameElm->block1.j;
                                break;
                            case 4:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j-2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i+2;
                                j_start = gameElm->block1.j+1;
                                break;
                        }
                    break;
                case 2:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i+2;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j;
                                break;
                            case 4:
                                i_start = gameElm->block1.i+2;
                                j_start = gameElm->block1.j-1;
                                break;
                            case 5:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j+2;
                                break;
                        }
                    break;
                case 3:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i-2;
                                j_start = gameElm->block1.j;
                                break;
                            case 4:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j+2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i-2;
                                j_start = gameElm->block1.j-1;
                                break;
                        }
                    break;
                }
                break;
            default:
                switch (gameElm->rotationState)
                {
                    case 0:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j-1;
                                break;
                            case 4:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j+2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j+2;
                                break;
                        }
                    break;
                case 1:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j+1;
                                break;
                            case 4:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j-2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j-2;
                                break;
                        }
                    break;
                case 2:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j-1;
                                break;
                            case 4:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j+2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i+1;
                                j_start = gameElm->block1.j+2;
                                break;
                        }
                    break;
                case 3:
                        switch (nTest)
                        {
                            case 1:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j;
                                break;
                            case 2:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j;
                                break;
                            case 3:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j+1;
                                break;
                            case 4:
                                i_start = gameElm->block1.i;
                                j_start = gameElm->block1.j-2;
                                break;
                            case 5:
                                i_start = gameElm->block1.i-1;
                                j_start = gameElm->block1.j-2;
                                break;
                        }
                    break;
                }
                break;
        }

        canTurn = 1;

        if (canTurn != 0)
        {
            for (j = 0; j < gameElm->dimension; j++)
            {
                for (i = 0; i < gameElm->dimension; i++)
                {
                    if (tetrimMap[i][j] == BLOCK_ACTIVE)
                    {
                        if (i_start+i < 0 || j_start+j < 0
                            || i_start+i >= NB_BLOCK_X || j_start+j >= NB_BLOCK_Y)
                            canTurn = 0;
                        else if (gameElm->gMap[i_start+i][j_start+j] == BLOCK_ACTIVE
                            || gameElm->gMap[i_start+i][j_start+j] == BLOCK_VOID)
                            continue;
                        else
                            canTurn = 0;
                    }
                }
            }
        }

        nTest++;

    } while (!canTurn && nTest <= 5);

    /* If the tetrimino can turn, copy the turned tetrim in the gMap */
    if (canTurn)
    {
        /* First, erase the actual tetrim */
        for (j = 0; j < gameElm->dimension; j++)
        {
            for (i = 0; i < gameElm->dimension; i++)
            {
                if (gameElm->block1.i+i < 0 || gameElm->block1.i+i >= NB_BLOCK_X
                    || gameElm->block1.j+j < 0 || gameElm->block1.j+j >= NB_BLOCK_Y)
                    continue;
                else if (gameElm->gMap[gameElm->block1.i+i][gameElm->block1.j+j] == BLOCK_ACTIVE)
                    gameElm->gMap[gameElm->block1.i+i][gameElm->block1.j+j] = BLOCK_VOID;
            }
        }

        /* Then copy the turn tetrim in the gMap */
        for (j = 0; j < gameElm->dimension; j++)
        {
            for (i = 0; i < gameElm->dimension; i++)
            {
                if (tetrimMap[i][j] == BLOCK_ACTIVE)
                    gameElm->gMap[i_start+i][j_start+j] = BLOCK_ACTIVE;
            }
        }

        gameElm->block1.i = i_start;
        gameElm->block1.j = j_start;

        gameElm->rotationState++;
        if (gameElm->rotationState == 4)
            gameElm->rotationState = 0;
    }

    for (i = 0; i < gameElm->dimension; i++)
    {
        free(tetrimMap[i]);
    }
    free(tetrimMap);

}

Uint8 locksTetrim (GameElements *gameElm)
{
    int activeBlock = 0, canFall = 1; /* Booleans */
    int i = 0, j = 0;

    /* Check whether there are some active blocks and whether they can go down lower */
    for (j = 0; j < NB_BLOCK_Y; j++)
    {
        for (i = 0; i < NB_BLOCK_X; i++)
        {
            if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
            {
                activeBlock = 1;
                if (j+1 < NB_BLOCK_Y)
                {
                    if (gameElm->gMap[i][j+1] != BLOCK_ACTIVE
                        && gameElm->gMap[i][j+1] != BLOCK_VOID )
                        canFall = 0;
                }
                else
                    canFall = 0;
            }
        }
    }

    /* If there is no active bloc, quits */
    if (!activeBlock)
        return 0;
    /* Else if there is an active tetrim and this one can fall, makes it fall until it gets on the stack */
    else if (activeBlock && canFall)
    {
        while (canFall)
        {
            canFall = ( !tetrimFalls(gameElm) );
            SDL_Delay (HARD_DROP_PERIOD);
        }
    }

    /* If there is an active tetrim and this one cannot fall lower, inactive it */
    if (activeBlock && !canFall)
    {
        for (j = 0; j < NB_BLOCK_Y; j++)
        {
            for (i = 0; i < NB_BLOCK_X; i++)
            {
                if (gameElm->gMap[i][j] == BLOCK_ACTIVE)
                {
                    switch (gameElm->actualTetrim)
                    {
                    case TETRIM_I:
                        gameElm->gMap[i][j] = BLOCK_CYAN;
                        break;
                    case TETRIM_O:
                        gameElm->gMap[i][j] = BLOCK_YELLOW;
                        break;
                    case TETRIM_T:
                        gameElm->gMap[i][j] = BLOCK_PURPLE;
                        break;
                    case TETRIM_L:
                        gameElm->gMap[i][j] = BLOCK_ORANGE;
                        break;
                    case TETRIM_J:
                        gameElm->gMap[i][j] = BLOCK_BLUE;
                        break;
                    case TETRIM_Z:
                        gameElm->gMap[i][j] = BLOCK_RED;
                        break;
                    case TETRIM_S:
                        gameElm->gMap[i][j] = BLOCK_GREEN;
                        break;
                    }
                }
            }
        }
        gameElm->tetrimActive = 0;
    }

    return checkCompleteLines(gameElm);
}

Uint8 checkCompleteLines (GameElements *gameElm)
{
    int i = 0, j = 0;
    Uint8 completeLine = 1;

    for (j = NB_BLOCK_Y-1; j >= 0; j--)
    {
        completeLine = 1;
        for (i = 0; i < NB_BLOCK_X; i++)
        {
            if (gameElm->gMap[i][j] == BLOCK_VOID || gameElm->gMap[i][j] == BLOCK_ACTIVE)
                completeLine = 0;
        }
        if (completeLine)
            return 1;
    }

    return 0;
}

void putTetrim (Uint32 field[4][4], int tetrim)
{
    int i, j;

    /* Erases every elements inside the field */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            field[i][j] = BLOCK_VOID;
        }
    }

    /* Paints the tetrim */
    switch (tetrim)
    {
        case TETRIM_I:
            field[0][2] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            field[3][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_O:
            field[1][1] = BLOCK_ACTIVE;
            field[2][1] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_T:
            field[1][1] = BLOCK_ACTIVE;
            field[0][2] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_J:
            field[0][1] = BLOCK_ACTIVE;
            field[0][2] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_L:
            field[2][1] = BLOCK_ACTIVE;
            field[0][2] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_Z:
            field[0][1] = BLOCK_ACTIVE;
            field[1][1] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            field[2][2] = BLOCK_ACTIVE;
            break;
        case TETRIM_S:
            field[1][1] = BLOCK_ACTIVE;
            field[2][1] = BLOCK_ACTIVE;
            field[0][2] = BLOCK_ACTIVE;
            field[1][2] = BLOCK_ACTIVE;
            break;
    }

}

