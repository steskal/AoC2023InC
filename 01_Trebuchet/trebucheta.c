#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
        int firstDigit  = 99;
        int secondDigit = 99;
        int idx         = 0;

        while(idx < read) {
           if(isdigit(line[idx])) {
               /*
                * convert char to string by substractin characte 0
                */
                if(firstDigit == 99) firstDigit = line[idx] - '0';
                secondDigit = line[idx] - '0';
            }
           idx++;
        }
        
        /*
         * In case the line is empty or does not contain any digits, dont add
         * to sum
         */
        if (firstDigit != 99) {
            sum += (firstDigit * 10) + secondDigit;
        }
    }
        printf("The sum is:\n%d\n", sum);

    return 0;
}
