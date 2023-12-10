#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>


#define NOOFPARAMS 2

typedef enum CardLetters {
        T = 10,
        J = 1,
        Q = 12,
        K = 13,
        A = 14
} CardLetters;

typedef struct Hand Hand;

struct Hand {
    unsigned short cards[5];
    unsigned short bet;
    /*
     * Type is how strong the hand is:
     * 6 - Five of a kind
     * 5 - Four of a kind
     * 4 - Full house
     * 3 - Thee of a kind
     * 2 - Two pair
     * 1 - One pair
     * 0 - High Card
     */
    unsigned short type;
    Hand *next;
};

CardLetters cardLetters;

static int              cardToValue(char cardLetter);
static void             addToFigerOutType(unsigned short toAdd);
static void             printFigureOutType(void);
static unsigned short   valueHand();
static void             placeHand(Hand *hand);
static short            compareHands(Hand *A, Hand *B);
static void             freeHands();
static unsigned long    calculateTotalWinnings(void);

// DEV HELPERS
static void printCards(Hand* hand);
static void printHandsOrder(void);
//~DEV HELPERS

Hand *handsHead = NULL;

unsigned short figureOutType[5][2] = {0};

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
        printf("Usage: <program> <data file>\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    char *line  = NULL; // To be used with getline() which takes care of alloc.
    size_t len  = 0;    // Size of buffer set by getline().
    ssize_t read;       // Return value of characters read by getline().
                        // -1 if EOF or failure to read the line.

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("File couldn't be opened\n");
        exit(EXIT_FAILURE);
    }

    while((read = getline(&line, &len, fp)) > 0) {

        /* Get rid of newline character at the end (if it is there) */
        char *newline = strrchr(line, '\n');
        *newline = '\0';
        
        /* split line to hand and bet */
        char *space = strchr(line, ' ');
        int lenToSpace = space - line;
        char *strHand = malloc(lenToSpace + 1 * sizeof(char));
        memset(strHand, '\0', lenToSpace * sizeof(char));
        strncpy(strHand, line, lenToSpace);

        /* store hand */
        Hand* newHand = malloc(sizeof(Hand));
        newHand->next = NULL;
        
        memset(figureOutType, 0, 10*sizeof(unsigned short));
        
        /* hand cannot have more than five cards */
        for(unsigned short idx = 0; idx < 5; idx++) {
            newHand->cards[idx] = cardToValue(*(strHand + idx));
            addToFigerOutType(newHand->cards[idx]);
        }
        
        /* calculate the value of the hand */
        newHand->type = valueHand();

        /* store bet */
        newHand->bet = atoi(space+1);

        /* insert hand into proper place in the linked list so there's no need
         * for sorting later */
        placeHand(newHand);

    }
    
    fclose(fp);
   
    /* calculate the final value */
    unsigned long totalWinnings = calculateTotalWinnings();

    printf("Result: %lu\n", totalWinnings);
    
    /* Free the list */
    freeHands();

    return 0;
}

/*
 * Convert card string to card unsigned short value.
 */
static int cardToValue(char cardLetter) {
    switch (cardLetter)  {
        case '2':
            return 2;
            break;
        case '3':
            return 3;
            break;
        case '4':
            return 4;
           break;
        case '5':
           return 5;
            break;
        case '6':
            return 6;
            break;
        case '7':
            return 7;
            break;
        case '8':
            return 8;
            break;
        case '9':
            return 9;
            break;
        case 'T':
            return T;
            break;
        case 'J':
            return J;
            break;
        case 'Q':
            return Q;
            break;
        case 'K':
            return K;
            break;
        case 'A':
            return A;
            break;
        default:
            printf("Not valid card: %c\n", cardLetter);
            return INT_MAX;
    }
}

/*
 * Add car to figureOutType 2-dimensional array. If alreade there,
 * just increment its counter.
 */
static void addToFigerOutType(unsigned short toAdd) {
    for(unsigned short idx = 0; idx < 5; idx++) {
        if(toAdd == figureOutType[idx][0]) {
            figureOutType[idx][1]++;
            return;
        }
        if(figureOutType[idx][0] == 0) {
            figureOutType[idx][0] = toAdd;
            figureOutType[idx][1]++;
            return;
        }
    }
}

/*
 * Calculate the value of the hand.
 */
static unsigned short valueHand() {
    unsigned short threeOfKind  = 0;
    unsigned short twoOfKind    = 0;

    for(unsigned short idx = 0; idx < 5; idx++) {
        /* Five and four of a kind would be firs in the array and also the only
         * type.
         */
        if(figureOutType[idx][1] == 5) return 6;
        if(figureOutType[idx][1] == 4) return 5;
        if(figureOutType[idx][1] == 3) threeOfKind++;
        if(figureOutType[idx][1] == 2) twoOfKind++;
    }

    if(threeOfKind == 1 && twoOfKind == 1) return 4;
    if(threeOfKind == 1) return 3;

    if(twoOfKind == 2) return 2;
    if(twoOfKind == 1) return 1;
        
    return 0;
}

static void placeHand(Hand *hand) {

    /* Putting firs hand into the list */
    if (handsHead == NULL) {
        handsHead = hand;
        return;
    }
    /* If putting element lower than handsHead */
        if(compareHands(hand, handsHead) == -1 ) {
            /* insert before */
            hand->next = handsHead;
            handsHead = hand;
            return;
        }    

    /* Putting other elements */
    Hand *previous   = handsHead;
    Hand *current    = previous->next;

    while(current != NULL && compareHands(hand, current) > -1 ) {
        
        previous = current;
        current = previous->next;
    }
    previous->next  = hand;
    if(current  != NULL) hand->next      = current;
}

/* Return:
 *  -1 if hand A is smaller
 *   0 if hand A is exactly same
 *   1 if hand A is better
 */
static short compareHands(Hand *A, Hand *B) {
    short result = 0;

    if (A->type < B->type) return -1;
    if (A->type > B->type) return  1;

    /* If here A->type must be equal to B->type */
    for(unsigned short idx = 0; idx < 5; idx++) {
        if(A->cards[idx] < B->cards[idx]) return -1;
        if(A->cards[idx] > B->cards[idx]) return  1;
    }

    return result;
}

/*
 * Calculate total winnings based on orde of hand multiplied by the bet. 
 */
static unsigned long calculateTotalWinnings(void) {
    unsigned long winnings  = 0;
    unsigned int counter    = 1;

    
    Hand *current = handsHead;

    while(current != NULL) {
        winnings += counter * current->bet;

        counter++;
        current = current->next;
    }
    
    return winnings;
}

/*
 * Clear memory from all the hjands in the list.
 */
static void freeHands() {
    Hand* current = handsHead;
    Hand* next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

// DEV HELPERS
static void printCards(Hand* hand) {
    for(unsigned short idx = 0; idx < 5; idx++) {
        printf("%hu ", hand->cards[idx]);
    }
    printf("\n");
}

static void printFigureOutType(void) {
    for(unsigned short idx = 0; idx < 5; idx++) {
        printf("Card: %hu, counter: %hu\n", figureOutType[idx][0],
                    figureOutType[idx][1]);
    }
}

static void printHandsOrder(void) {

    Hand *current = handsHead;

    while(current != NULL) {
        printf("Card\n========\n");
        printCards(current);
        printf("Hand Type: %hu\n", current->type);
        printf("Hand bet: %hu\n", current->bet);
        printf("--------\n\n");

        current = current->next;
    }
}
//~DEV HELPERS
