#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOOFPARAMS 2

#ifdef TEST
#define HISTORYLEN 6
#else
#define HISTORYLEN 21
#endif

int sequence[HISTORYLEN][HISTORYLEN] = {0};

long getNextInSeq(unsigned short startRow);
unsigned short allSequenceZero(unsigned short rowIox);

// DEV HELPERS
void printOneSeq(unsigned short row);
//~DEV HELPERS

int main(int argc, char *argv[]) {

    long sum = 0;

    if(argc != NOOFPARAMS) {
        printf("Usage: <program> <data_file> \n");
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

        char *newline = strrchr(line, '\n');
        *newline = '\0';

        memset(sequence, 0, HISTORYLEN * HISTORYLEN * sizeof(int));
        
        unsigned short counter = 0;
        char *token = strtok(line, " ");
        while(token != NULL) {
            sequence[0][counter] = atoi(token);
            counter++;
            token = strtok(NULL, " ");
        }

        sum += getNextInSeq(0);
   }
    
    fclose(fp);

    printf("Result: %lu \n", sum);

    return 0;
}

/* Gen next number in sequnce of the row from sequence[][] */
long getNextInSeq(unsigned short startRow) {

    if(allSequenceZero(startRow)) return 0;
    
    unsigned short thisSeqCount = HISTORYLEN - startRow - 1;
    
    for(unsigned short idx = 0; idx < thisSeqCount; idx++) {
        sequence[startRow + 1][idx] = sequence[startRow][idx + 1] -
                                    sequence[startRow][idx];
    }

// TEST
// For some reaoso removing the followng printf maks this code produce 
// wrong result
printf("");
//~TEST
    
    long ret = sequence[startRow][0] - getNextInSeq(startRow + 1);
    return ret;
}

/*
 * Check if all nummbers in sequence are 0.
 */
unsigned short allSequenceZero(unsigned short rowIdx) {
    unsigned int wcount = 0;
    for(unsigned short idx; idx < HISTORYLEN - rowIdx; idx++) {
        if(sequence[rowIdx][idx] != 0) wcount++;
    }

    if(wcount == 0) return 1;
    return 0;
}

// DEV HELPERS
void printOneSeq(unsigned short row) {
    printf("Printing row %hu.\n", row);
    for(unsigned short idx = 0; idx < HISTORYLEN; idx++) {
        printf("%d ", sequence[row][idx]);
    }
    printf("\n");
}
//~DEV HELPERS
