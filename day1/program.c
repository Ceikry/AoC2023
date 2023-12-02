#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PARSEBUG    '\0'
#define LOOKUPNUM   10

typedef struct {char *key; char val;} t_lookup;

static t_lookup lookupTable[] = {
    { "zero",   '0' },
    { "one",    '1' },
    { "two",    '2' },
    { "three",  '3' },
    { "four",   '4' },
    { "five",   '5' },
    { "six",    '6' },
    { "seven",  '7' },
    { "eight",  '8' },
    { "nine",   '9' }
};

static bool partTwo = true;
static bool verbose = false;

int procLn (char ln[], int numChars);
char chkStr (char string[]);
void insertChar (char* backing, int backingSize, char value);

int main (int argc, char* argv[]) {
    FILE *fp;
    char *buff = NULL;
    size_t buff_size = 128;

    size_t read;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-1") == 0)
            partTwo = false;
        else if (strcmp(argv[i], "-v") == 0)
            verbose = true;
        else if (fp == NULL && strstr(argv[i], ".txt") != NULL) {
            printf ("Using file: %s\n", argv[i]);
            fp = fopen (argv[i], "r");
        }
    }

    if (fp == NULL) {
        printf ("No file passed, trying input.txt in current folder.\n");
        fp = fopen ("input.txt", "r");
    }

    if (fp == NULL)
            exit (EXIT_FAILURE);

    int _sum = 0;
    while (read = getline(&buff, &buff_size, fp)) 
    {
        if (read == -1) break;
        buff[read - 1] = '\0';
        if (verbose)
            printf("Read %d chars -> '%s'\n", read, buff);
        _sum += procLn (buff, read);
        if (verbose)
            printf("Total = %d\n", _sum);
    }

    if (!verbose)
        printf ("Total = %d\n", _sum);
}

int procLn (char ln[], int numChars) {
    char nums[] = { -1, -1, '\0' };
    char curStr[5] = {[0 ... 4] = ' '};

    for (int i = 0; i < numChars; i++) {
        char thisNum;

        if (partTwo) {
            insertChar (curStr, 5, ln[i]);
            thisNum = chkStr (curStr);
        } else thisNum = PARSEBUG;

        if (thisNum == PARSEBUG) {
            if (ln[i] < '0' || ln[i] > '9') {
                continue;
            }
            thisNum = ln[i];
        }

        if (nums[0] == -1)
            nums[0] = thisNum;
        nums[1] = thisNum;
    }

    if (verbose)
        printf ("I think this is right -> %s\n", nums);
    return atoi (nums);
}

char chkStr (char string[]) {
    t_lookup *opt;
    char* result;

    if (verbose)
        printf("Checking: %s\n", string);

    for (int i = 0; i < LOOKUPNUM; i++) {
        opt = lookupTable + i;
        result = strstr(string, opt->key);
        if (result != NULL) {
            if (verbose)
                printf("Found: %s\n", opt->key);
            *result = '0';
            return opt->val;
        }
    }
    return PARSEBUG;
}

void insertChar (char* backing, int backingSize, char value) {
    for (int i = 0; i < backingSize; i++) {
        if (i == backingSize - 1) {
            *(backing + i) = value;
        } else {
            *(backing + i) = *(backing + i + 1);
        }
    }
}
