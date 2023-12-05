#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define NOOFPARAMS      2
#define START_TICKETS 198

int winning[100];
int tickets[START_TICKETS];

static void          markWinningNumbers(char *winningNumbers);
static unsigned int  getTicketPoints(char *ticketNumbers);
static void          populateTickets(int winningTicket, int winCount);
static unsigned int  countTickets(void);

int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
        printf("Usage: scrat <data file>\n");
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

    memset(winning, 0, 100*sizeof(int));
    memset(tickets, 0, 198*sizeof(int));

    int ticketNo = 0;

    while((read = getline(&line, &len, fp)) > 0) {

        char *winningNoPart, *ticketNoPart;
        char *pipe;

        /* Add ticket counter for current ticekt in ticets array 
         * Remember the value could have been incremented before by winning 
         * tickets so don't set it to valu, just increment */
        tickets[ticketNo]++;

        /* Move after the "Game *: " */
        char *tmp = strchr(line, ':') + 2;

        /* Get rid od endind new line character */
        char *newLine = strrchr(tmp, '\n');
        *newLine = '\0';

        /* Split to two tokens by "|". 
         * There are only 2 parts-> winning numbers and ticket numbers */
        pipe = strchr(tmp, '|');
        int lenToPipe = (pipe - tmp);
        int lenAfterPipe = strlen(pipe+1);
        
        winningNoPart = malloc(lenToPipe * sizeof(char));
        memset(winningNoPart, '\0', lenToPipe * sizeof(char));
        strncpy(winningNoPart, tmp, lenToPipe -1);

        ticketNoPart = malloc(lenAfterPipe);
        memset(ticketNoPart, '\0', lenAfterPipe);
        strncpy(ticketNoPart, pipe + 2, lenAfterPipe - 1);  // Also get rid of
                                                            // the space after 
                                                            // the pipe.
        /* Process ticket */
        markWinningNumbers(winningNoPart);
        populateTickets( ticketNo, getTicketPoints(ticketNoPart));

        memset(winning, 0, 100*sizeof(int)); // Reset for next ticket
        ticketNo++;
    }

    printf("Result B: %u\n", countTickets());
    
    fclose(fp);

    return 0;
}

/* Split the string by spaces to individual numbers and mark them 
 * in the winning array. */
static void markWinningNumbers(char *winningNumbers) {

    char *numbers = winningNumbers;
    numbers = strtok(numbers, " ");

    while(numbers != NULL) {
        
        unsigned short currNo = atoi(numbers);

        if(currNo == 0) {
            printf("This is not a valid winning number: \"%s\"\n", numbers);
            exit(EXIT_FAILURE);
        }

        winning[currNo] = 1;

        numbers = strtok(NULL, " ");
    }

}

/* Check each ticket number if it matches winning number and calculate
 * total points per ticket */
static unsigned int  getTicketPoints(char *ticketNumbers) {

    unsigned int found = 0;
    
    /* Start splitting. Tcket nubers are devided by space */
    char *tmp = strtok(ticketNumbers, " ");

    while(tmp != NULL) {

        unsigned short currNo = atoi(tmp);
        if(currNo == 0) {
            printf("This is not a valid ticket number: \"%s\"\n", tmp);
            exit(EXIT_FAILURE);
        }
        if(winning[currNo] > 0) {
            found++;
        }

        tmp = strtok(NULL, " ");
    }

    return found;
}

/* Add one copy of next winCount tickets after winningTicket */
static void populateTickets(int winningTicket, int winCount) { 
    for(int tidx = winningTicket+1; tidx <= winningTicket + winCount; tidx++) {
        tickets[tidx] += tickets[winningTicket] ;
    }
}

/* Count all the tickets */
static unsigned int  countTickets(void) {
    unsigned int count = 0;
    for(int idx = 0; idx < START_TICKETS; idx++) {
        count += tickets[idx];
    }
    return count;
}

