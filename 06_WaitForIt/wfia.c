#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOOFPARAMS 2
#define NOOFRACES  4

unsigned short input[NOOFRACES][2] = {0}; // There are 4 races each storing time 
                                  // and distance 
                                  
static unsigned int calculateRace(unsigned short raceTime, unsigned short recordDistance);

int main(int argc, char *argv[]) {

unsigned int noOfWays = 0;
    
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

    unsigned short lineCounter   = 0;

    /* Parse input */
    while((read = getline(&line, &len, fp)) > 0) {
        if(lineCounter > 1 && isalnum(*line)) {
            printf("Too many lines in the file: %hu\n", lineCounter);
            exit(EXIT_FAILURE);
        }

        char *tmp = strchr(line, ':') + 1;
        tmp = strtok(tmp, " ");
        unsigned short numberCounter = 0;
        while(tmp != NULL) {
            if(numberCounter >= NOOFRACES) {
                printf("Too many races it seems: %hu\n", numberCounter);
                exit(EXIT_FAILURE);
            }

            if(isdigit(*tmp)) {
                input[numberCounter][lineCounter] = atoi(tmp);
                numberCounter++;
            }

            tmp = strtok(NULL, " ");
        }

        lineCounter++;
    }

    fclose(fp);
    
    for(unsigned short racex = 0; racex < NOOFRACES; racex++) {
        if(input[racex][0] == 0) break; // If no of races is smaller, than defined
        if(noOfWays == 0) {
            noOfWays += calculateRace(input[racex][0], input[racex][1]);
        }else{
            noOfWays *= calculateRace(input[racex][0], input[racex][1]);
        }
    }

    printf("Result: %u\n", noOfWays);

    return 0;
}

/* Calculate number of winning options for one race */
static unsigned int calculateRace(unsigned short raceTime, unsigned short recordDistance) {
    unsigned short countWinning = 0;

    for(unsigned short charge = 1; charge < raceTime - 1; charge++) {
        unsigned int distance = (raceTime - charge) * charge;
        if (distance > recordDistance) countWinning++;
    }


    return countWinning;
}
