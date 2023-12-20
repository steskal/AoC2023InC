#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
    #define INWIDTH 10
    #define GALAXYEXPAND 99
#else
    #define INWIDTH 140
    #define GALAXYEXPAND 999999
#endif

#define NOOFPARAMS 2

typedef struct Galaxy Galaxy;

struct Galaxy {
    unsigned short  no;
    unsigned long  x;
    unsigned long  y;
    Galaxy         *next;

};

Galaxy          *galaxiesHead = NULL;
unsigned short   columns[INWIDTH] = {0};

void            adjustYAxes(Galaxy *head);
unsigned long   sumGalaxyDistances(Galaxy *start);
void            freeGalaxies(Galaxy *head);

// DEV HELPERS
void printGalaxyList(Galaxy *head);
void printColumnsMap(void);
//~DEV HELPERS

int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
        printf("Usage: <program> <data file> \n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    char *line  = NULL; // To be used with getline() which takes care of alloc.
    size_t  len  = 0;    // Size of buffer set by getline().
    ssize_t read;       // Return value of characters read by getline().
                        // -1 if EOF or failure to read the line.

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("File couldn't be opened\n");
        exit(EXIT_FAILURE);
    }

    unsigned short galaxyCounter   = 0;
    unsigned long rowCounter      = 0;
    Galaxy        *lastAddedGalaxy = NULL;

    while((read = getline(&line, &len, fp)) > 0) {
        char c                      = *line;
        unsigned short charPos      = 0;
        unsigned short flagEmptyRow = 1;

        while(c != '\n' && c != '\0') {
            if(c == '#') {
                Galaxy *newG = (Galaxy*) malloc(sizeof(Galaxy));
                newG->no    = ++galaxyCounter;
                newG->x     = rowCounter;
                newG->y     = charPos;
                newG->next  = NULL;

                if(galaxiesHead == NULL) {
                    galaxiesHead    = newG;
                    lastAddedGalaxy = newG;
                }else{
                    lastAddedGalaxy->next   = newG;
                    lastAddedGalaxy         = newG;
                }
                flagEmptyRow     = 0;
                columns[charPos] = 1;
            }

            charPos++;
            c = *(line+charPos);

        }

        if(flagEmptyRow) rowCounter += GALAXYEXPAND;
        rowCounter++;
    }

    fclose(fp);
    
    adjustYAxes(galaxiesHead);

    unsigned long total = 0;
    Galaxy *curr = galaxiesHead;
    while(curr != NULL) {
        total += sumGalaxyDistances(curr);
        curr = curr->next;
    }

    printf("Result: %lu\n", total);

    freeGalaxies(galaxiesHead);

    return 0;
}

/*
 * Adjust Y axis for expanding universe.
 */
void adjustYAxes(Galaxy *head) {
    Galaxy *current = head;
    while(current != NULL) {
        unsigned long limit = current->y;
        for(unsigned short idx = 0; idx < limit; idx++) {
            if(columns[idx] == 0) current->y += GALAXYEXPAND;
        }
        current = current->next;
    }
}

/*
 * Calculate all distances for one galaxy.
 * Only calculate distances with galaxies with higher no.
 * to preven for calculating one distance twice.
 */
unsigned long sumGalaxyDistances(Galaxy *start) {
    Galaxy *step = start->next;
    unsigned long sum = 0;
    while(step != NULL) {
        sum += abs(step->x - start->x) + abs(step->y - start->y);
        step = step->next;
    }
    return sum;
}

/*
 * Free memory from all galaxies in the linked list.
 */
void freeGalaxies(Galaxy *head) {
    Galaxy *curr = head;
    Galaxy *next = NULL;
    while(curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    head = NULL;

}


// DEV HELPERS
void printGalaxyList(Galaxy *head) {
    Galaxy *current = head;
    while(current != NULL) {
        printf("Galaxy no: %hu, x: %lu, y: %lu\n", current->no, current->x,
                        current->y);
        current = current->next;
    }
}

void printColumnsMap(void) {
    printf("Columns Map: ");
    for(unsigned short idx = 0; idx < INWIDTH; idx++) {
        printf("%hu ", columns[idx]);
    }
    printf("\n");
}
//~DEV HELPERS
