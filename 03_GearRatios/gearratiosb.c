#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Number Number;
typedef struct Gear Gear;

struct Number {
    unsigned long   number;
    unsigned int    rowIdx;
    unsigned int    idxStart;
    unsigned int    idxEnd;
    Number         *previous;
};

struct Gear  {
    unsigned int    rowIdx;
    unsigned int    columnIdx;
    Gear           *previous;
};

Number *tail   = NULL;
Gear *gearTail = NULL;

static void     processLine(char *line, unsigned int rowIdx, int rowchar, int *map);
static char*  processNumber(char *noStart, char *lineStart, unsigned int rowIdx);
static unsigned int sumPartNumbers(Number *tail, unsigned int rowCount, unsigned int rowchar, int *map);
static unsigned int sumMapPart(unsigned int start, unsigned int end, int* map);
static unsigned int findGearRatios(Number *numTail, Gear *gearTail);
static          int checkGearColumnRange(Number *num, Gear *gear);

/* Dev helpers */
static void printNumbers();
static void printSymbolMap(int *map, int noOfRows, int noOfCharsInRow);
/*~Dev helpers */

/*
 * Data file as parameter of the program. Expecting to compile to executable:
 * gera.
 * 1st parameter is <data file>
 * 2nd paramete should be no of rows in the document. Do not count empty rows.
 * 3rd parameter nomber of characters per row - don't count \n
 */
int main(int argc, char **argv) {

    /* Not going to check if counts of lines and characters in line 
     * is correct */

    if(argc != 4) {
        printf("Usage: gera <datafilename> <no of rows> <no of charas in row>\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    char *line  = NULL; // To be used with getline() which takes care of alloc.
    size_t len  = 0;    // Size of buffer set by getline().
    ssize_t read;       // Return value of characters read by getline().
                        // -1 if EOF or failure to read the line.
    
    unsigned int rowIdx     = 0;
    int rowcount            = atoi(argv[2]);
    int charinrowcount      = atoi(argv[3]);        
    int arraySymbolMap[rowcount][charinrowcount];
    /* Fill array with zeros and later odify only the fields that need it */
    memset(arraySymbolMap, 0, rowcount * charinrowcount * sizeof(int));

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("File couldn't be opened\n");
        exit(EXIT_FAILURE);
    }

    while((read = getline(&line, &len, fp)) > 0) {
        processLine(line, rowIdx, charinrowcount, (int*)arraySymbolMap);
        memset(line, '\0', strlen(line)*sizeof(char));
        rowIdx++;
    }

    unsigned int partSum = sumPartNumbers(tail, rowcount, charinrowcount, (int*) arraySymbolMap);

    printf("Sum of parts is: %u\n", partSum);

    unsigned int ratios = findGearRatios(tail, gearTail);

    printf("Sum of rations is: %u\n", ratios);

    fclose(fp);
}

/*
 * Walk through each character of the line.
 */
static void processLine(char *line, unsigned int rowIdx, int rowchar, int *map) {
    char *tmp = line;   // use to walk
    
    /* Remember lasst line does not need to have new line  but all the rest do*/
    while(*tmp != '\0' && *tmp != '\n') {
        int currentPosInRow = tmp - line;

        if(isdigit(*tmp)) {
            tmp = processNumber(tmp, line, rowIdx);
            continue;
        }
        if(*tmp == '*') {
            Gear *newGear       = malloc(sizeof(Gear));
            newGear->rowIdx     = rowIdx;
            newGear->columnIdx  = currentPosInRow;
            newGear->previous   = gearTail;
            gearTail  = newGear;
        }
        if(*tmp != '.') *(map + (rowIdx * rowchar) + currentPosInRow) = 1;
        tmp++;
    }
}

/*
 * Add number to list and return pointer to next character after number
 */
static char* processNumber(char* noStart, char* lineStart, unsigned int rowIdx) {

    char *next = NULL;
    /* Create new tail in link */
    Number *newTail     = malloc(sizeof(Number));
    newTail->previous   = tail;
    tail                = newTail;
    
    /* Set the value and indexes of number. */
    newTail->number     = strtol(noStart, &next, 10);
    newTail->idxStart   = (unsigned int)(noStart - lineStart);
    newTail->idxEnd     = (unsigned int)(next - 1 -lineStart);
    newTail->rowIdx     = rowIdx;


    return next;
}

static unsigned int sumPartNumbers(Number *tail, unsigned int rowCount, unsigned int rowchar, int *map) {
    unsigned int partSum = 0;
    
    Number *current = tail;

    while(current != NULL) {
        /* the idea is that if there was any character except . and \n around
         * the number, this will end up to be > 0
         */
        int mapSum  = 0;
        int toStart = 0;
        int toEnd   = 0;
        
        /* Get the square aroung left corner and width. Tak in account map 
         * borders */
        int left    = current->idxStart - 1;
        if(left < 0) left = 0;
        toStart = (current->rowIdx) * rowchar + left;
        int right   = current->idxEnd + 1;
        if(right < rowchar -1) {
            toEnd = right + 1  - left;
        }else{
            toEnd = (rowchar - left);
        }
        
        /* test row below */
        if(current->rowIdx < rowCount - 1) { 
            mapSum += sumMapPart(toStart + rowchar, toEnd, map);
        }

        /* test same row */
        mapSum += sumMapPart(toStart, toEnd, map);

        /* test row above */
        if(current->rowIdx > 0) { 
           mapSum += sumMapPart(toStart - rowchar, toEnd, map);
        }

        if(mapSum > 0) partSum += current->number;
        /* move to next number */
        current = current->previous;
    }

    

    return partSum;
}

static unsigned int sumMapPart(unsigned int start, unsigned int end, int* map) {
    unsigned int sum = 0;
    int *tmp = map + start;
        
        for(int idx = 0; idx < end; idx++){
            sum += *(tmp+idx);
        }

    return sum;
}

static unsigned int findGearRatios(Number *numTail, Gear *gearTail) {
    unsigned int ratios = 0;

    /* Go through all gears and check if there are two numbers connecting.
     * Both list are starting from last position in the source data and moving
     * to the begining of the data. Try to at least break the cycle once you
     * start checking numbers that have rowIdx < (gear->rowIdx - 1).
     * first check row, ond move on if it is not in range -> bit faster */

    Gear *currentG = gearTail;
    
    /* Go through found * */
    while(currentG != NULL) {

        Number * currentN = numTail;
        
        Number *found[2];
        found[0]         = NULL;
        found[1]         = NULL;
        int foundCounter = 0;
        
        /* Go through numbers to find if they are in range */
        while(currentN != NULL) {
            if(currentG->rowIdx -1 > currentN->rowIdx) break; // Skip some rows
            if(currentN->rowIdx >= currentG->rowIdx -1 
                    && currentN->rowIdx <= currentG->rowIdx +1) {

                if(checkGearColumnRange(currentN, currentG)) {
                    if(++foundCounter > 2) break;
                    found[foundCounter -1] = currentN;
                }

            }
            
            currentN = currentN->previous;
        }

        if(foundCounter == 2) {
            ratios += found[0]->number * found[1]->number;
        }

        currentG = currentG->previous;
    }
    
    return ratios;
}

/* Check if number's column is in range for * */
static int checkGearColumnRange(Number *num, Gear *gear) {
    int isInRange = 0;  //start with false
    
    if(num->idxEnd >= gear->columnIdx -1 && num->idxStart <= gear->columnIdx +1) {
        isInRange = 1;
    }
    return isInRange;
}

/* Dev helpers */

/* 
 * Prnit nubers in reverse order compared to file. Last in file, first in print.
 */
static void printNumbers() {
    Number* currentNo = tail;
    
    while(currentNo != NULL) {
        printf("Number: %lu, row index: %u, start index: %u, end inex: %u\n",
                    currentNo->number, currentNo->rowIdx, currentNo->idxStart,
                    currentNo->idxEnd);
        currentNo = currentNo->previous;
    }
}

static void printSymbolMap(int *map, int noOfRows, int noOfCharsInRow) {

    for(int rowIdx = 0; rowIdx < noOfRows; rowIdx++) {
        for(int charIdx = 0; charIdx < noOfCharsInRow; charIdx++) {
            printf("%d", *(map + (rowIdx * noOfCharsInRow) + charIdx));
        }
        printf("\n");
    }
}

/*~Dev helpers */

