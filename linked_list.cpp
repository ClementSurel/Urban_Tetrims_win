#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "linked_list.h"
#include "constants.h"
#include "game.h"

LNK_List* LNK_createBag ()
{
    LNK_List *LNK_list = (LNK_List*) malloc(sizeof(*LNK_list));
    if (LNK_list == NULL)
    {
        fprintf(stderr, "An unknown error occurred during memory allocation for the LNK_List\n");
        return NULL;
    }

    LNK_list->firstElm = NULL;

    return LNK_list;
}

void LNK_freeList (LNK_List *LNK_list)
{
    Element *actualElm = NULL;
    Element *nextElm = NULL;

    if (LNK_list == NULL)
        return;

    if (LNK_list->firstElm == NULL)
    {
        free(LNK_list);
        return;
    }

    nextElm = LNK_list->firstElm;
    free(LNK_list);

    while (nextElm != NULL)
    {
        actualElm = nextElm;
        nextElm = nextElm->next;
        free (actualElm);
    }

}

void LNK_printList (LNK_List *LNK_list)
{
    Element *actualElm = NULL;
    int i = 1;

    if (LNK_list == NULL)
    {
        fprintf(stdout, "The pointer on the LNK_List is null\n");
        return;
    }

    if (LNK_list->firstElm == NULL)
    {
        fprintf(stdout, "None element found in the LIFO_list\n");
        return;
    }

    actualElm = LNK_list->firstElm;

    while (actualElm != NULL)
    {
        fprintf(stdout, "The element n. %d is %d\n", i, actualElm->tetrim);
        actualElm = actualElm->next;
        i++;
    }

}

void LNK_addElement (LNK_List *LNK_list, int tetrim)
{
    Element *newElm = (Element*)malloc(sizeof(*newElm));

    if (newElm == NULL)
    {
        fprintf(stderr, "Error occurred during memory allocation for a new element\n");
        return;
    }

    if (LNK_list == NULL)
    {
        fprintf(stdout, "The pointer on the LNK_List is null\n");
        free (newElm);
        return;
    }

    newElm->tetrim = tetrim;
    newElm->next = LNK_list->firstElm;
    LNK_list->firstElm = newElm;

}

int LNK_drawTetrim (LNK_List *LNK_list)
{
        /* Variables */
    int i = 0;
    int tetrim = 0, nbElm = 0, nElmToPick = 0;
    Element *actualElm = NULL, *elmToSuppr = NULL;

        /* Checks whether the list is filled or empty */
    if (LNK_list == NULL)
    {
        fprintf(stderr, "The pointer on the LIFO_List is null\n");
        return 0;
    }

    if (LNK_list->firstElm == NULL)
    {
        fprintf(stdout, "None element found in the LIFO_List\n");
        return 0;
    }

        /* Picks a random number depending on the number of resting element */
    nbElm = LNK_getNbElm (LNK_list);
    nElmToPick = SDL_GetTicks() % nbElm;

        /* Looks for the element to pick and gets the tetrimino */
    elmToSuppr = LNK_list->firstElm;
    for (i = 0; i < nElmToPick; i++)
    {
        actualElm = elmToSuppr;
        elmToSuppr = elmToSuppr->next;
    }
    tetrim = elmToSuppr->tetrim;

        /* Deletes the picked element */
    if (actualElm == NULL)
        LNK_list->firstElm = elmToSuppr->next;
    else
        actualElm->next = elmToSuppr->next;

    free (elmToSuppr);

    return tetrim;
}

int LNK_getNbElm (LNK_List *LNK_list)
{
        /* Variables */
    int nbElm = 0;
    Element *actualElm = NULL;

        /* Checks whether the list exists or not */
    if (LNK_list == NULL)
    {
        fprintf(stdout, "Pointer on LIFO_list has a NULL value\n");
        return 0;
    }
        /* Counts the number of elements and returns it */
    actualElm = LNK_list->firstElm;

    while (actualElm != NULL)
    {
        nbElm++;
        actualElm = actualElm->next;
    }

    return nbElm;
}

void LNK_fillBag (LNK_List *bag)
{
    if (bag == NULL)
        return;

    LNK_addElement(bag, TETRIM_I);
    LNK_addElement(bag, TETRIM_O);
    LNK_addElement(bag, TETRIM_L);
    LNK_addElement(bag, TETRIM_J);
    LNK_addElement(bag, TETRIM_Z);
    LNK_addElement(bag, TETRIM_S);
    LNK_addElement(bag, TETRIM_T);

}

