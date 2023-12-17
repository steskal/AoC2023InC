#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define NOOFPARAMS 2

/* Input matrix without newline chars is square */
#ifdef TEST
    #define INSIZE 5
#else
    #define INSIZE 140
#endif

typedef struct Coordinates Coordinates;
struct Coordinates {
    int x;
    int y;
};

Coordinates *start = NULL;

char maze[INSIZE][INSIZE + 1] = {'\0'};

Coordinates *findFirstStep(Coordinates *start);
Coordinates *findNextStep(Coordinates *currentTile, Coordinates *previousTile);

// DEV HELPERS
void printMaze(void);
// DEV HELPERS

int main(int argc, char *argv[]) {

    unsigned long stepCounter = 0;
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

    int lineCounter = 0;


    while((read = getline(&line, &len, fp)) > 0) {
        strncpy(&maze[lineCounter][0], line, INSIZE * sizeof(char));
        char *startLoc = strchr(line, 'S');
        if(startLoc != NULL) {
            start = malloc(sizeof(Coordinates));
            start->x = lineCounter;
            start->y = startLoc - line;
        }
       lineCounter++;
    }

    fclose(fp);
    
    /* Find first path out - start North and go clockwis. Take first usable 
     * path out */
    Coordinates *nextTile = findFirstStep(start);
    Coordinates *previousTile = start;
    stepCounter++;

    while(nextTile->x != start->x || nextTile->y != start->y) {
        Coordinates *current = nextTile;
        nextTile = findNextStep(current,  previousTile);

        if(previousTile->x != start->x || previousTile->y != start->y) {
            free(previousTile);
        }
        previousTile = current;
        stepCounter++;

    }
    printf("Result: %lu\n", stepCounter / 2);
    
    return 0;
}

/* 
 * Find the first way out of start - start from North and move clockwise.
 * Return coordinates of the next tile on the way.
 */
Coordinates *findFirstStep(Coordinates *start) {
    Coordinates *next = malloc(sizeof(Coordinates));
    char tile = '.';

    /* Go North */
    if(start->x > 0) {
        tile = maze[start->x - 1][start->y];
        if(strchr("|7F", tile) != NULL) {
                next->x = start->x - 1;
                next->y = start->y;
                return next;
        }
    }
    /* Go East */
    if(start->y < INSIZE -1) {
        tile = maze[start->x][start->y + 1];
        if(strchr("-J7", tile) != NULL) {
                next->x = start->x;
                next->y = start->y + 1;
                return next;
        }
    }
    /* Go South */
    if(start->x < INSIZE - 1) {
        tile = maze[start->x + 1][start->y];
        if(strchr("|LJ", tile) != NULL) {
            next->x = start->x + 1;
            next->y = start->y;
            return next;
        }
    }
    /* Go West */
    if(start->y > 0) {
        tile = maze[start->x][start->y - 1];
        if(strchr("-LF", tile) != NULL) {
            next->x = start->x;
            next->y = start->y;
            return next;
        }
    }
    /* No way out if we end here */
    return NULL;
}

/*
 * Find next tile on the way
 */
Coordinates *findNextStep(Coordinates *currentTile, Coordinates *previousTile) {
    Coordinates *next = malloc(sizeof(Coordinates));
    switch(maze[currentTile->x][currentTile->y]) {
        case '|':
            next->y = currentTile->y;
            if(previousTile->x < currentTile->x) {
                next->x = currentTile->x + 1;
                return next;
            }
            next->x = currentTile->x - 1;
            return next;

        case '-':
            next->x = currentTile->x;
            if(previousTile->y < currentTile->y) {
             next->y = currentTile->y + 1;
                return next;
            }
            next->y = currentTile->y - 1;
            return next;

        case 'L':
            if(previousTile->x == currentTile->x) {
                next->x = currentTile->x - 1;
                next->y = currentTile->y;
                return next;
            }
            next->x = currentTile->x;
            next->y = currentTile->y + 1;
            return next;

        case 'F':
            if(previousTile->x == currentTile->x) {
                next->x = currentTile->x + 1;
                next->y = currentTile->y;
                return next;
            }
            next->x = currentTile->x;
            next->y = currentTile->y + 1;
            return next;

        case '7':
            if(previousTile->x == currentTile->x) {
                next->x = currentTile->x + 1;
                next->y = currentTile->y;
                return next;
            }
            next->x = currentTile->x;
            next->y = currentTile->y - 1;
            return next;

        case 'J':
            if(previousTile->x == currentTile->x) {
                next->x = currentTile->x - 1;
                next->y = currentTile->y;
                return next;
            }
            next->x = currentTile->x;
            next->y = currentTile->y - 1;
            return next;
        default:
            break;
    }

    return next;
}

// DEV HELPERS
void printMaze(void) {
    for(unsigned short idx = 0; idx < INSIZE; idx++) {
        printf("%s\n", &maze[idx][0]);
    }
}
//~DEV HELPERS
