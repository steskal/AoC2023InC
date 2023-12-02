#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

/*
 * Comment following line to switch between testing data set and full data set.
 */
//#define TEST 1

#ifdef TEST
    const char *src = "test_input.dat";
#endif

#ifndef TEST
    const char *src = "full_input.dat";
#endif

static void checkWord(const char *haystack, const char *needle, int value);

int wordPositions[2];
int wordValues[2];

int main() {

    int sum = 0;
    
    FILE *fp;
    char *line  = NULL;      // To be used with getline() which takes care of alloc.
    size_t len  = 0;         // Size of buffer set by getline().
    ssize_t read;           // Return value of characters read by getline().
                            // -1 if EOF or failure to read the line.
    
    fp = fopen(src, "r");
    if(fp == NULL) exit(EXIT_FAILURE);

    while((read = getline(&line, &len, fp)) > 0) {
        int idx         = 0;
        
        wordPositions[0] = INT_MAX;
        wordPositions[1] = -1;
        wordValues[0]    = -1;
        wordValues[1]    = -1;

        /*
         * Search for word occurences.
         */
        checkWord(line, "one",      1);
        checkWord(line, "two",      2);
        checkWord(line, "three",    3);
        checkWord(line, "four",     4);
        checkWord(line, "five",     5);
        checkWord(line, "six",      6);
        checkWord(line, "seven",    7);
        checkWord(line, "eight",    8);
        checkWord(line, "nine",     9);


        while(idx < read) {
           if(isdigit(line[idx])) {
               /*
                * convert char to string by substractin characte 0
                */
               
                if(idx < wordPositions[0]) {
                    wordPositions[0] = idx;
                    wordValues[0] = line[idx] - '0';
                }

                if(idx > wordPositions[1]) {
                    wordPositions[1] = idx;
                    wordValues[1] = line[idx] - '0';
                }
            }
           idx++;
        }
        
        /*
         * In case the line is empty or does not contain any digits, dont add
         * to sum
         */
        if(wordPositions[0] < INT_MAX) {
            sum += (wordValues[0] * 10) + wordValues[1];
        }
    }

        printf("The sum is:\n%d\n", sum);

    return 0;
}

/*
 * Update firs and last occurrence of the word in haystack and update also its
 * value in respective arrays.
 */
static void checkWord(const char *haystack, const char *needle, int value) {

    const char *tmp = haystack;

    while((tmp = strstr(tmp, needle))) {
        int found = (int)(tmp - haystack);
        if(found < wordPositions[0]) {
            wordPositions[0]    = found;
            wordValues[0]       = value;
        }

        if(found > wordPositions[1]) {
            wordPositions[1] = found;
            wordValues[1]   = value;
        }

        tmp++;
    }    
}
