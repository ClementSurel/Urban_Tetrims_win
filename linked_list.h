/** linked_list.h and linked_list.cpp manage a linked list

    According to the tetris rules, the generation of the tetriminoes is not totally random. In fact,
    all the 7 different tetriminoes must be generated in a row, but the order is random, as if they were drawn
    from a bag.
    In order to follow this rule, a linked list is used. At the beginning of the game, the list is
    filled with the 7 tetriminoes in a given order but each time the game needs a new tetrimino,
    one of them is taken from the list in a random way.
**/

#ifndef LNK_LIST_H_INCLUDED
#define LNK_LIST_H_INCLUDED

typedef struct Element Element;
typedef struct LNK_List LNK_List;

struct Element
{
    int tetrim;

    Element *next;
};

struct LNK_List
{
    Element *firstElm;
};


/** Creates the list **/
LNK_List* LNK_createBag ();

/** Frees the list **/
void LNK_freeList (LNK_List*);

/** Prints the list in the stdout file.
    This function can be called to check eventual errors. Not called in the final version of the game **/
void LNK_printList (LNK_List*);

/** Add an element in the list (in other words, a tetrimino in the bag) **/
void LNK_addElement (LNK_List *bag, int tetrim);

/** Removes an element from the list (picks up a tetrimino from the bag) **/
int LNK_drawTetrim (LNK_List*);

/** Returns the number of elements in the list (number of remaining tetriminoes in the bag) **/
int LNK_getNbElm (LNK_List*);

/** Fills the list with all the 7 differents elements (fills the bag with the 7 different tetriminoes **/
void LNK_fillBag (LNK_List *bag);

#endif // LIFO_LIST_H_INCLUDED
