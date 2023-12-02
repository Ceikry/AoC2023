#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PARSEBUG   -1
#define COLORNUM    3
#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2

const int MAX_AMT[] = { 12, 13, 14 };

typedef struct {char *key; int index;} t_index_lookup;
typedef struct {int index; int amt; bool setDone;} t_colres;
typedef struct {bool setValid; bool gameDone;} t_setres;

static t_index_lookup colorLookup[] = {
    { "red",    INDEX_RED   },
    { "green",  INDEX_GREEN },
    { "blue",   INDEX_BLUE  }
};

int procLn (char line[], size_t count);
int getGameNum (char line[], int* i);
t_colres ingestColor (char line[], int* i);
t_setres validSet (char line[], int* i, size_t count, int* highest);
bool validGame (char line[], int* i, size_t count, int* highest);
bool isDigit (char c);
bool isLetter (char c);
bool isTerm (char c);

bool verbose = false;
bool partTwo = false;

int main (int argc, char* argv[])
{
    FILE *fp;
    char *buff = NULL;
    size_t buff_size = 512;

    size_t read;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-2") == 0)
            partTwo = true;
        else if (strcmp(argv[i], "-v") == 0)
            verbose = true;
        else if (fp == NULL && strstr(argv[i], ".txt") != NULL)
        {
            printf ("Using file: %s\n", argv[i]);
            fp = fopen (argv[i], "r");
        }
    }

    if (fp == NULL)
    {
        printf ("No file passed, trying input.txt in current folder.\n");
        fp = fopen ("input.txt", "r");
    }

    if (fp == NULL)
            exit (EXIT_FAILURE);

    int _sum = 0;
    while (read = getline(&buff, &buff_size, fp)) 
    {
        if (read <= 1) break;
        buff[read - 1] = '\0';
        if (verbose)
            printf("Read %d chars -> '%s'\n", (int) read, buff);
        _sum += procLn (buff, read);
        if (verbose)
            printf("Total = %d\n", _sum);
    }
    if (!verbose)
        printf("Total = %d\n", _sum);
}

bool isDigit (char c)
{
    return c >= '0' && c <= '9';
}

bool isLetter (char c)
{
    return c >= 'a' && c <= 'z';
}

bool isTerm (char c)
{
    return c == '\0' || c == '\n';
}

int procLn (char line[], size_t count)
{
    int gameNum;

    int i = 0;
    gameNum = getGameNum (line, &i);
    
    if (!partTwo)
    {
        if (validGame(line, &i, count, NULL))
            return gameNum;
        else
            return 0;
    } 
    else 
    {
        int highest[] = { 0, 0, 0 };
        validGame(line, &i, count, highest);
        return highest[0] * highest[1] * highest[2];
    }
}

int getGameNum (char line[], int* i)
{
    char gameNum[4] = {[0 ... 3] = '\0'};
    int gi = 0;
    
    char c;
    for (; c != ':'; (*i)++)
    {
        c = line[*i];
        if (isDigit(c))
        {
            gameNum[gi++] = c;
        }
    }

    if (verbose)
        printf ("Found Game Number: %s\n", gameNum);

    return atoi (gameNum);
}

bool validGame (char line[], int* i, size_t count, int* highest)
{
    t_setres sInfo = { true, false };

    if (verbose)
        printf("Starting game validation at index %d\n", *i);

    while (!sInfo.gameDone)
    {
        sInfo = validSet (line, i, count, highest);
        if (verbose && !partTwo)
        {
            printf ("Set Valid? %s\n", sInfo.setValid ? "true" : "false");
        }
        if (!sInfo.setValid)
            return false;
    }
    return true;
}

t_setres validSet (char line[], int* i, size_t count, int* highest)
{
    int setSums[] = { 0, 0, 0 };
    bool lastSet = false;

    if (verbose)
        printf("Starting set validation at index: %d\n", *i);

    t_colres cInfo = { 0, false };
    while (!cInfo.setDone)
    {
        cInfo = ingestColor (line, i);
        if (cInfo.index == PARSEBUG)
        {
            printf ("Error parsing, got PARSEBUG. Continuing.\n");
            continue;
        }

        setSums[cInfo.index] += cInfo.amt;

        if (verbose && !partTwo)
        {
            printf (
                    "Cur Sums: [%d,%d,%d]\nMax Sums: [%d,%d,%d]\n",
                    setSums[0], setSums[1], setSums[2],
                    MAX_AMT[0], MAX_AMT[1], MAX_AMT[2]
            );
        }

        lastSet = *i >= count - 1;

        if (!partTwo)
        {
            if (setSums[cInfo.index] > MAX_AMT[cInfo.index])
                return (t_setres) { false, lastSet };
        }
    }

    if (partTwo)
    {
        for (int j = 0; j < COLORNUM; j++)
        {
            if (setSums[j] > *(highest + j))
                *(highest + j) = setSums[j];
        }

        if (verbose)
        {
            printf (
                    "Cur Sums: [%d,%d,%d]\nMax Sums: [%d,%d,%d]\n",
                    setSums[0], setSums[1], setSums[2],
                    *(highest + 0), *(highest + 1), *(highest + 2)
            );
        }
    }

    return (t_setres) { true, lastSet };
}

t_colres ingestColor (char line[], int* i)
{
    t_index_lookup *ctype;
    char color[6] = {[0 ... 5] = ' '};
    int _ci = 0;
    char amt[4] = {[0 ... 3] = ' '};
    int _ai = 0;

    if (verbose)
    {
        printf("Parsing color at index %d\n", *i);
    }

    char c = ':';
    for (; c != ',' && c != ';' && !isTerm(c); (*i)++)
    {
        c = line[*i];

        if (isDigit(c))
            amt[_ai++] = c;
        else if (isLetter(c))
            color[_ci++] = c;
    }

    amt[_ai] = '\0';
    color[_ci] = '\0';

    if (verbose)
    {
        printf ("Col: %s\nAmt: %s\n", color, amt);
    }

    int cIndex = PARSEBUG;
    for (int j = 0; j < COLORNUM; j++)
    {
        ctype = colorLookup + j;
        if (strcmp(ctype->key, color) == 0)
        {
            cIndex = ctype->index;
            break;
        }
    }

    return (t_colres)
    {
        cIndex,
        atoi(amt),
        c == ';' || isTerm(c)
    };
}
