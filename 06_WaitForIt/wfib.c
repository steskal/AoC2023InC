#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOOFPARAMS 2

unsigned long raceDuration   = 0;
unsigned long recordDistance = 0;
                                  
static unsigned long calculateRace(unsigned long raceTime, unsigned long recordDistance);

int main(int argc, char *argv[]) {

unsigned long noOfWays = 0;
    
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
    char numbuffer[20];
    memset(numbuffer, '\0', 20*sizeof(char));


    /* Parse input */
    while((read = getline(&line, &len, fp)) > 0) {
        if(lineCounter > 1 && isalnum(*line)) {
            printf("Too many lines in the file: %hu\n", lineCounter);
            exit(EXIT_FAILURE);
        }

        char *tmp = strchr(line, ':') + 1;
        tmp = strtok(tmp, " ");
        //unsigned short numberCounter = 0;
        while(tmp != NULL) {
            if(isdigit(*tmp)) 
               strcat(numbuffer, tmp);

            tmp = strtok(NULL, " ");
        }
        
        if(lineCounter == 0) raceDuration = atol(numbuffer);
        else if(lineCounter == 1) recordDistance = atol(numbuffer);
        memset(numbuffer, '\0', 20*sizeof(char));

        lineCounter++;
    }

    fclose(fp);

    noOfWays = calculateRace(raceDuration, recordDistance);

    printf("Result: %lu\n", noOfWays);
    
    return 0;
}

/* Calculate number of winning options for one race */
static unsigned long calculateRace(unsigned long raceTime, unsigned long recordDistance) {
    unsigned long countWinning = 0;

    for(unsigned long charge = 1; charge < raceTime - 1; charge++) {
        unsigned long distance = (raceTime - charge) * charge;
        if (distance > recordDistance) countWinning++;
    }

    return countWinning;
}
