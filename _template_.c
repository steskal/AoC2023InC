#include <stdio.h>
#include <stdlib.h>


#define NOOFPARAMS 2

int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
//TODO: Add  usage string
        printf("Usage: \n");
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
//TODO: Add some code here -> replace the printf
        printf("%s", line);
    }

//TODO: Add result text to printf
    printf("Result: \n");
    
    fclose(fp);

    return 0;
}
