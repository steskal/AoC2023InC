#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Some notes about full input:
 * instructions (LR...) have 270 char with '\n' and seems to have 726 lines without 
 * intructions and separator line.
 *
 * Size of a pointer to Step structure seems to be 8 bytes on MacOS on M1 Mac.
 */
#define NOOFPARAMS          2

typedef struct Node Node;

struct Node {
    /* Chars are only for dev/test purposes to print it human readable */
    char name[4];
    char left[4];
    char right[4];
    
    /* this should store numerical value for order of the character in English
     * aphabet. Values 0-25.
     * The order in array is the same asin the string representation.
     */
    int  iName[3];
    int  iLeft[3];
    int  iRight[3];
};

char  instructions[271]     = {'\0'};
Node *nodesMap[25][25][25]  = {0};


Node          *parseNode(const char *line);
unsigned long  countSteps(void);

int main(int argc, char *argv[]) {
    
    if(argc != NOOFPARAMS) {
        printf("Usage: <program> <data file>\n");
        exit(EXIT_FAILURE);
    } 

    FILE   *fp;
    char   *line  = NULL; // To be used with getline() which takes care of alloc.
    size_t  len   = 0;    // Size of buffer set by getline().
    ssize_t read;         // Return value of characters read by getline().
                          // -1 if EOF or failure to read the line.

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("File couldn't be opened\n");
        exit(EXIT_FAILURE);
    }

    /* Read first line */
    read = getline(&line, &len, fp); 
    if(read <= 0) {
        printf("File is empty.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(instructions, line);

    /* Read second line with separator empty line */
    read = getline(&line, &len, fp);

    while((read = getline(&line, &len, fp)) > 0) {
        Node *newNode = parseNode(line);
        nodesMap[newNode->iName[0]][newNode->iName[1]][newNode->iName[2]] = newNode;
    }

    fclose(fp);

    /* Count the steps */
    unsigned long steps = countSteps();

    printf("Result: %lu\n", steps);
    

    return 0;
}

/*
 * Parse the Node object from sting in file.
 */
Node *parseNode(const char *line) {
    Node* newNode = malloc(sizeof(Node));

    /* 1st 3 chars are node name */
    memset(newNode->name, '\0', 4 * sizeof(char));
    strncpy(newNode->name, line, 3);
    
    /* moveto start of left instruction and parse it */
    line += 7;
    memset(newNode->left, '\0', 4 * sizeof(char));
    strncpy (newNode->left, line, 3);

    /* Move to start of right instruction and parse it */
    line += 5;
    memset(newNode->right, '\0', 4 * sizeof(char));
    strncpy(newNode->right, line, 3);

    /* convert string to number coordinates */
    newNode->iName[0] = (int)*(newNode->name)     - 65;
    newNode->iName[1] = (int)*(newNode->name + 1) - 65;
    newNode->iName[2] = (int)*(newNode->name + 2) - 65;
   
    newNode->iLeft[0] = (int)*(newNode->left)     - 65;
    newNode->iLeft[1] = (int)*(newNode->left + 1) - 65;
    newNode->iLeft[2] = (int)*(newNode->left + 2) - 65;

    newNode->iRight[0] = (int)*(newNode->right)     - 65;
    newNode->iRight[1] = (int)*(newNode->right + 1) - 65;
    newNode->iRight[2] = (int)*(newNode->right + 2) - 65;

    return newNode;
}

/*
 * Count the steps.
 */
unsigned long countSteps(void) {
    unsigned long steps= 0;
    
    /* starting at AAA */
    Node *currentNode = nodesMap[0][0][0];
    char *currInstruction = &instructions[0];

    while(strcmp(currentNode->name, "ZZZ") != 0) {
   
        if(*(currInstruction) == 'L') {
            currentNode = nodesMap[currentNode->iLeft[0]][currentNode->iLeft[1]]
                                [currentNode->iLeft[2]];
        } else {
            currentNode = nodesMap[currentNode->iRight[0]]
                            [currentNode->iRight[1]][currentNode->iRight[2]];
        }

        steps++;

        if(*(currInstruction + 1) == '\n') {

            /* Make sure you are looping through instructions */
            currInstruction = &instructions[0];
        } else {
            currInstruction++;
        }
    }

    return steps;
}
