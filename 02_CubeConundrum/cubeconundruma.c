#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ONLY_RED    12
#define ONLY_GREEN  13
#define ONLY_BLUE   14

/* some constants for parsing */
#define GAME_EXTENDED_LEN   5  // Length of string Game + space
#define RED_LEN     3          // Length of string red
#define GREEN_LEN   5          // Length of string green
#define BLUE_LEN    4          // Length of string blue

static int processLine(char *line);

/*
 * Pass path to datafile for processing as the argument of the program.
 */
int main(int argc, char **argv) {

    /*
     * Expecting exactly 2 arguments for this program.
     * Keep in mind unix systems are passing the comand used to invoke the 
     * program as first argument.
     */

    if(argc != 2) {
        printf("No data file was passed as argument.\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    char *line  = NULL; // To be used with getline() which takes care of alloc.
    size_t len  = 0;    // Size of buffer set by getline().
    ssize_t read;       // Return value of characters read by getline().
                        // -1 if EOF or failure to read the line.
    int sum = 0;        // The sum of the num of possible games.
    

    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("File couldn't be opened.\n");
        exit(EXIT_FAILURE);
    }
    
    while((read = getline(&line, &len, fp)) > 0) {

        sum += processLine(line);
        /* Clean the buffer, the next line can be shortr and leave
         * artefacts from this buffer causing issues.
         */
        memset(line, '\0', strlen(line)*sizeof(char));

    }

    printf("The sum is: %d\n", sum); 

    fclose(fp);    

    return 0;
}

static int processLine(char *line) {
    
    int result = 0;
    int gameNo = 0;
    int redHigh = 0, greenHigh = 0, blueHigh = 0;
    size_t lenToDelim = 0;
    char number[5];     // That should be large enough
    
    memset(number, '\0', 5 * sizeof(char));

    /* Get game number */
    char *tmp = line + GAME_EXTENDED_LEN; // Move after "Game "
    lenToDelim = strchr(tmp, ':') - tmp;
    strncpy(number, tmp, lenToDelim);

    gameNo = atoi(number);
    
    /* Clean and move to firs cube number */
    tmp += lenToDelim + 2; // There is always space after :
    lenToDelim = 0;
    memset(number, '\0', 5 * sizeof(char));
    
    /* Loop through bricks */
    while(*tmp != '\0') {
        /* Get number */
        int currentNumber = 0;
        int moveBy = 0;

        /* Get number */
        lenToDelim = strchr(tmp, ' ') - tmp;
        strncpy(number, tmp, lenToDelim);
        currentNumber = atoi(number);

        tmp += lenToDelim + 1; //always a space after number
        
        /* Get colour */
        switch (tmp[0]) {
            case 'r':   // RED
                if(currentNumber > redHigh) redHigh = currentNumber;
                /* Move to next brick unless this is a last one */
                moveBy = (*(tmp+RED_LEN) == '\0' ? RED_LEN : RED_LEN + 2);

                break;

            case 'g':   // GREEN
                if(currentNumber > greenHigh) greenHigh = currentNumber;
                /* Move to next brick unless this is a last one */
                moveBy = (*(tmp+GREEN_LEN) == '\0' ? GREEN_LEN : GREEN_LEN + 2);

                break;

            case 'b':   // BLUE
                if(currentNumber > blueHigh) blueHigh = currentNumber;
                /* Move to next brick unless this is a last one */
                moveBy = (*(tmp+BLUE_LEN) == '\0' ? BLUE_LEN : BLUE_LEN + 2);

                break;

            default:    // Error
                printf("Unexpected coulour or string.\n");
                printf("Buffer: \"%s\"\n", tmp);
                exit(EXIT_FAILURE);
        }   

        /* Clean and move to next colour */
        memset(number, '\0', 5 * sizeof(char));
        tmp += moveBy;
    }
    
    if(redHigh <= ONLY_RED && greenHigh <= ONLY_GREEN && blueHigh <= ONLY_BLUE)
        result = gameNo;
    
    return result;
}
