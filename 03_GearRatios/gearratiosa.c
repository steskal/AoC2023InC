#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Number Number;
struct Number{
    unsigned long   number;
    unsigned int    rowIdx;
    unsigned int    idxStart;
    unsigned int    idxEnd;
    Number         *previous;
};

Number *tail = NULL;

static void     processLine(char *line, unsigned int rowIdx, int rowchar, int *map);
static char*  processNumber(char *noStart, char *lineStart, unsigned int rowIdx);
static unsigned int sumPartNumbers(Number *tail, unsigned int rowCount, unsigned int rowchar, int *map);
static unsigned int sumMapPart(unsigned int start, unsigned int end, int* map);

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
        if(*tmp != '.') *(map + (rowIdx*rowchar) + currentPosInRow) = 1;
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

