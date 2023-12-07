#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define NOOFPARAMS 2
#define NOOFSEEDPAIRS 10
#define NOOFSEEDS 10000000    //NEED to be set manually before compilation

/* Array of input seeds */
unsigned int seeds[NOOFSEEDPAIRS * 2];
unsigned int paths[NOOFSEEDS][8] = {0}; 

typedef struct Map Map;

struct Map {
    unsigned int srcStart;
    unsigned int targetStart;
    unsigned int span;
    Map         *next;
};

/* Linked lists for each Map list */
Map *seed2soilHead      = NULL;
Map *soil2fertHead      = NULL;
Map *fert2waterHead     = NULL;
Map *water2lightHead    = NULL;
Map *light2tempHead     = NULL;
Map *temp2humidHead     = NULL;
Map *humid2locHead      = NULL;

static void         extractSeeds(char *seedStr);
static void         addMap(char *str, Map **head);
static void         addToPaths(int inPathIdx, Map *head);
static unsigned int findClosestiLocation();
static void         freeList(Map *head);
static void         runCalculation();
static unsigned int fillSeedsIntoPaths(unsigned int start, unsigned int span);

// DEV HELPERS
static void printSeeds();
static void printMap(Map *head);
static void printPaths();
//~DEV HELPERS

int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
        printf("Usage: <program name> <data file>\n");
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
    
    /* Indicate in which section we are - seeds are first.
     *
     * The order of sections is same in test_file.dat and full_file.dat,
     * so we can just iterate through them:
     *  0 - Seeds - these are the only once on the same line
     *  1 - seed-to-soil map
     *  2 - soil-to-fertilizer map
     *  3 -fertilizer-to-water map
     *  4 - water-to-light map
     *  5 - light-to-temperature map
     *  6 - temperature-to-humidity map
     *  7 - humidity-to-location map
     * */
    short switchSection = -1;

    /* Map the document into linked lists */
    while((read = getline(&line, &len, fp)) > 0) {

        /* Skip empty lines */
        if(read == 1) continue;
        
        /* Detect section switching */
        if(isalpha(*line)) {
            /* the orde is always the same and we have alredy got rid of empty
             * lines. If this is alpha, go to next section. Except for seeds
             * that have numbvers on the same line as section header.*/
            switchSection++;
            if(switchSection > 0 ) continue;
        }

        char *tmp = NULL;

        switch (switchSection) {
            case 0:
               tmp = (strchr(line, ':') + 2);
               extractSeeds(tmp);
                break;
            case 1:
                addMap(line, &seed2soilHead);
                break;
            case 2:
                addMap(line, &soil2fertHead);
                break;
            case 3:
                addMap(line, &fert2waterHead);
                break;
            case 4:
                addMap(line, &water2lightHead);
                break;
            case 5:
                addMap(line, &light2tempHead);
                break;
            case 6:
                addMap(line, &temp2humidHead);
                break;
            case 7:
                addMap(line, &humid2locHead);
                break;
            default:
                break;
        }
    }

    fclose(fp);

    runCalculation();

    freeList(seed2soilHead);
    freeList(soil2fertHead);
    freeList(fert2waterHead);
    freeList(water2lightHead);
    freeList(light2tempHead);
    freeList(temp2humidHead);
    freeList(humid2locHead);

    return 0;
}

/* Extract seeds into array */
static void extractSeeds(char *seedStr) {
    seedStr = strtok(seedStr, " ");
    unsigned short counter = 0;

    while(seedStr != 0) {
        seeds[counter] = atoi(seedStr);
        seedStr = strtok(NULL, " ");
        counter++;
    }
}

/* Take string with map, make it to struct Map and put it to list 
 * NOTE  the Heads are pointers - storing address of first Map - I need
 * to pass addres of the Head, otherwise the head would not be updated to point 
 * the newly added Map -> pointer to pointer is parameter */
static void addMap(char *str, Map **head) {

    Map *newMap =(Map*) malloc(sizeof(Map));
    unsigned short counter = 0;
    str = strtok(str, " ");
    while(str != NULL) {
        switch (counter) {
            case 0:
               newMap->targetStart = atoi(str);
               break;
            case 1:
                newMap->srcStart = atoi(str);
                break;
            case 2:
                newMap->span = atoi(str);
              break;
            default:
                printf("Unexpected number of map tokens. \n");
                exit(EXIT_FAILURE);
        }
        
        str = strtok(NULL, " ");
        counter++;
    }
    newMap->next = *head;
    *head = newMap;
}

/* inPathIdx point so current part of path updated - if getting target seed 
 * to soil -> 1 storing the target value of the map seed-to-soil: 
 * 0 - seeds
 * 1 - seed-to-soil
 * 2 - soil-to-fertilizer map
 * ...
 * 7 - humidity-to-location
 * 
 * Why am I looping through linked list first and then wice through array?
 * Most of the lists are larger than no of seeds and looping through list is 
 * much slower tahn looping through array*/
static void addToPaths(int inPathIdx, Map *head) {
   
    /* Loop through the linked list */
    while (head != NULL) {
 
        unsigned int srcEnd     = head->srcStart + head->span -1;
        for (unsigned int idx = 0; idx < NOOFSEEDS; idx++) {
            
            unsigned int mapFor = paths[idx][inPathIdx -1];

            if(mapFor >= head->srcStart && mapFor <= srcEnd) {
                unsigned int mapTo = head->targetStart + 
                                        (mapFor - head->srcStart);
                paths[idx][inPathIdx] = mapTo;

            }
        }

        head = head->next;
    }

    /* If still 0 set it to same value as source */
    for(unsigned int idy = 0; idy < NOOFSEEDS; idy++) {
        if(paths[idy][inPathIdx] == 0)
            paths[idy][inPathIdx] = paths[idy][inPathIdx - 1];
    }
}

static unsigned int findClosestiLocation() {
    unsigned int closest = 0;
    for(unsigned int idy = 0; idy < NOOFSEEDS; idy++) {
        if(closest == 0) {
            closest = paths[idy][7];
            continue;
        }
        if(paths[idy][7] < closest & paths[idy][0] != 0) 
            closest = paths[idy][7];
    }
    return closest;
}

static void freeList(Map *head) {
    Map* current = head;
    while (current != NULL) {
        Map *next = current->next;
        free(current);
        current = next;
    }
}

/* Step through calculationa in parts - array for paths is smaler than 
 * nober of seeds in ieach seed-span pair */
static void runCalculation() {
    int seedSpanPairs [NOOFSEEDPAIRS][2] = {0};
    unsigned int closestLocation = 0;

    /* First split parsed seeds line into pars of start and span */
    for(unsigned short seedx = 0; seedx < NOOFSEEDPAIRS * 2; seedx++) {
        seedSpanPairs[seedx][0] = seeds[2* seedx];
        seedSpanPairs[seedx][1] = seeds[2*seedx + 1];
    }

    /* NOw go through all seed-span pairs */
    for(unsigned short seedx = 0; seedx < NOOFSEEDPAIRS; seedx++) {
        unsigned int currTopSpan = 0;
        unsigned int lastProcessedNo = seedSpanPairs[seedx][0];
        unsigned int limit = 0;
        if(seedSpanPairs[seedx][0]+seedSpanPairs[seedx][1] > 0) limit = seedSpanPairs[seedx][0]+seedSpanPairs[seedx][1] -1;
        while(lastProcessedNo < limit)  {
            unsigned int remindingSpan = seedSpanPairs[seedx][0] + seedSpanPairs[seedx][1] - lastProcessedNo;
            if(remindingSpan < NOOFSEEDS) currTopSpan = remindingSpan;
            else currTopSpan = NOOFSEEDS;
            lastProcessedNo = fillSeedsIntoPaths(lastProcessedNo, currTopSpan);
            lastProcessedNo++;
            
            addToPaths(1, seed2soilHead);
            addToPaths(2, soil2fertHead);
            addToPaths(3, fert2waterHead);
            addToPaths(4, water2lightHead);
            addToPaths(5, light2tempHead);
            addToPaths(6, temp2humidHead);
            addToPaths(7, humid2locHead);
            unsigned int closestInbatch = findClosestiLocation();
            if(closestInbatch < closestLocation || closestLocation == 0) 
                closestLocation = closestInbatch;

            memset(paths, 0, sizeof(paths));
        }
    }
    
    /* Print Result */
    printf("Result: %u\n", closestLocation);
}


static unsigned int fillSeedsIntoPaths(unsigned int start, unsigned int span) {
    unsigned int lastProcessed = 0;

    for(unsigned int arrIdx = 0; arrIdx < span; arrIdx++) {
        lastProcessed = start + arrIdx;
        paths[arrIdx][0] = lastProcessed;
    }

    return lastProcessed;
}

// DEV HELPERS
static void printSeeds() {

    for(int idx = 0; idx < 20; idx++) {
        printf("%u ", paths[idx][0]);
    }

    printf("\n");

}


static void printMap(Map *head) {
   
    while (head != NULL) {
        printf("Target: %u, Source: %u, Span: %u\n", head->targetStart,
                    head->srcStart, head->span);
        head = head->next;
    }
    
}

static void printPaths() {
    printf("Paths\n=====\n");
    for(unsigned int idy = 0; idy < NOOFSEEDS; idy++) {
        printf("Path %hu: ", idy);
        for(unsigned short idx = 0; idx < 8; idx ++) {
            printf("%d ", paths[idy][idx]);
        }
    printf("\n");
    }
}

//~DEV HELPERS
