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
//Coordinates *pathEnd = NULL;

char maze[INSIZE][INSIZE + 1]                = {'\0'};
int  map[(2 * INSIZE) + 1][(2 * INSIZE) + 1] = {0};

Coordinates   *findFirstStep(Coordinates *start);
Coordinates   *findNextStep(Coordinates *currentTile, Coordinates *previousTile);
unsigned short rayTrace(void);
unsigned int   countInLoopTiles(void);

// DEV HELPERS
void printMaze(void);
void printMap(void);
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
       // printf("%s", line);
       lineCounter++;
    }

    fclose(fp);

    /* Add start to map
     */
    map[(2 * start->x) + 1][(2 * start->y) + 1] = 1;
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
    
    /* Mark fields that are not part of the pipe loop and can access border
     * without crossing any pipe in the loop.
     */
    unsigned short changed = 0;
    do {
        changed = rayTrace();
    } while(changed != 0);
    unsigned int inLoopCounter = countInLoopTiles();
    
    printf("Result: %lu\n", stepCounter / 2);
    printf("In loop tiles: %u\n", inLoopCounter);

    return 0;
}

/* 
 * Find the first way out of start - start from North and move clockwise.
 * Return coordinates of the next tile on the way.
 */
Coordinates *findFirstStep(Coordinates *start) {
    Coordinates *next = malloc(sizeof(Coordinates));
    char tile = '.';
    unsigned short mapXnext = 0;
    unsigned short mapYnext = 0;
    /* Go North */
    if(start->x > 0) {
        tile = maze[start->x - 1][start->y];
        if(strchr("|7F", tile) != NULL) {
                next->x = start->x - 1;
                next->y = start->y;
                /* add map */
                mapXnext = (next->x * 2) + 1;
                mapYnext = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext + 1][mapYnext] = 1;
                return next;
        }
    }
    /* Go East */
    if(start->y < INSIZE -1) {
        tile = maze[start->x][start->y + 1];
        if(strchr("-J7", tile) != NULL) {
                next->x = start->x;
                next->y = start->y + 1;
                /* add map */
                mapXnext = (next->x * 2) + 1;
                mapYnext = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext][mapYnext - 1] = 1;
                return next;
        }
    }
    /* Go South */
    if(start->x < INSIZE - 1) {
        tile = maze[start->x + 1][start->y];
        if(strchr("|LJ", tile) != NULL) {
                next->x = start->x + 1;
                next->y = start->y;
                /* add map */
                mapXnext = (next->x * 2) + 1;
                mapYnext = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext - 1][mapYnext] = 1;
                return next;
        }
    }
    /* Go West */
    if(start->y > 0) {
        tile = maze[start->x][start->y - 1];
        if(strchr("-LF", tile) != NULL) {
                next->x = start->x;
                next->y = start->y;
                /* add map */
                mapXnext = (next->x * 2) + 1;
                mapYnext = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext + 1][mapYnext] = 1;
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
    unsigned short mapXnext = 0;
    unsigned short mapYnext = 0;

    switch(maze[currentTile->x][currentTile->y]) {
        case '|':
            next->y  = currentTile->y;
            mapYnext = (next->y * 2) + 1;

            if(previousTile->x < currentTile->x) {
                next->x                     = currentTile->x + 1;
                mapXnext                    = (next->x * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext - 1][mapYnext] = 1;
                return next;
            }
            next->x                     = currentTile->x - 1;
            mapXnext                    = (next->x * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext + 1][mapYnext] = 1;
            return next;

        case '-':
            next->x = currentTile->x;
            mapXnext = (next->x *2) + 1;

            if(previousTile->y < currentTile->y) {
                next->y                     = currentTile->y + 1;
                mapYnext                    = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext][mapYnext - 1] = 1;
                return next;
            }
            next->y                     = currentTile->y - 1;
            mapYnext                    = (next->y * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext][mapYnext + 1] = 1;
            return next;

        case 'L':
            if(previousTile->x == currentTile->x) {
                next->x                     = currentTile->x - 1;
                next->y                     = currentTile->y;
                mapXnext                    = (next->x * 2) + 1;
                mapYnext                    = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext + 1][mapYnext] = 1;
                return next;
            }
            next->x                     = currentTile->x;
            next->y                     = currentTile->y + 1;
            mapXnext                    = (next->x * 2) + 1;
            mapYnext                    = (next->y * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext][mapYnext - 1] = 1;
            return next;

        case 'F':
            if(previousTile->x == currentTile->x) {
                next->x                     = currentTile->x + 1;
                next->y                     = currentTile->y;
                mapXnext                    = (next->x * 2) + 1;
                mapYnext                    = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext - 1][mapYnext] = 1;
                return next;
            }
            next->x                     = currentTile->x;
            next->y                     = currentTile->y + 1;
            mapXnext                    = (next->x * 2) + 1;
            mapYnext                    = (next->y * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext][mapYnext - 1] = 1;
            return next;

        case '7':
            if(previousTile->x == currentTile->x) {
                next->x                     = currentTile->x + 1;
                next->y                     = currentTile->y;
                mapXnext                    = (next->x * 2) + 1;
                mapYnext                    = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext - 1][mapYnext] = 1;
                return next;
            }
            next->x                     = currentTile->x;
            next->y                     = currentTile->y - 1;
            mapXnext                    = (next->x * 2) + 1;
            mapYnext                    = (next->y * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext][mapYnext + 1] = 1;
            return next;

        case 'J':
            if(previousTile->x == currentTile->x) {
                next->x                     = currentTile->x - 1;
                next->y                     = currentTile->y;
                mapXnext                    = (next->x * 2) + 1;
                mapYnext                    = (next->y * 2) + 1;
                map[mapXnext][mapYnext]     = 1;
                map[mapXnext + 1][mapYnext] = 1;
                return next;
            }
            next->x                     = currentTile->x;
            next->y                     = currentTile->y - 1;
            mapXnext                    = (next->x * 2) + 1;
            mapYnext                    = (next->y * 2) + 1;
            map[mapXnext][mapYnext]     = 1;
            map[mapXnext][mapYnext + 1] = 1;
            return next;
        default:
            break;
    }

    return next;
}

/* 
 * Find and mark tiles that can be get to border without crossing pipes in loop.
 * Skip tiles that are pipe in loope.
 */
unsigned short rayTrace(void) {
    unsigned short changed = 0;

    for(unsigned short idx = 0; idx < (2 * INSIZE) + 1; idx++) {
        for(unsigned short idy = 0; idy < (2 * INSIZE) + 1; idy++) {
            if(map[idx][idy] == 1 || map[idx][idy] == 2) continue;
            /* Sees border */
            if(idx - 1 < 0 || idy - 1 < 0 || idx + 1 > (2 * INSIZE) 
                    || idy + 1 > (2 * INSIZE)) {
                map[idx][idy] = 2;
                changed++;
            }
            /* Sees other tile that has path to border without crosing the pipe
             * and is not a pipe itself */
            if(        map[idx - 1][idy] == 2 
                    || map[idx][idy + 1] == 2
                    || map[idx + 1][idy] == 2
                    || map[idx][idy - 1] == 2
              ) {
                map[idx][idy] = 2;
                changed++;
            }
        }
    }
    return changed;
}

/*
 * Count tiles that are enclosed inside the loop.
 */

unsigned int countInLoopTiles(void) {
    unsigned int count = 0;
    /* Skip the interleaving rows and count onlyrows that would be in original
     * maze.
     */
    for(unsigned short idx = 1; idx < (2 * INSIZE) + 1; idx += 2) {
        for(unsigned short idy = 1; idy < (2 * INSIZE) + 1; idy += 2) {
            if(map[idx][idy] == 0) count++;
        }
    }
    return count;
}

// DEV HELPERS
void printMaze(void) {
    for(unsigned short idx = 0; idx < INSIZE; idx++) {
        printf("%s\n", &maze[idx][0]);
    }
}


void printMap(void) {
    printf("Printing map: \n");
    for(unsigned short idx = 0; idx < (2 * INSIZE) + 1; idx++) {
        for(unsigned short idy = 0; idy < (2 * INSIZE) + 1; idy++) {
            printf("%d ", map[idx][idy]);
        }
        printf("\n\n");
    }
}

//~DEV HELPERS
