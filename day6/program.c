#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#define KEY_COUNT 2
#define KEY_TIME "Time"
#define KEY_DIST "Distance"

bool verbose = false;
bool partTwo = false;

typedef struct {char *key; uint64_t *map;} t_pmap;

void readWord (char line[], size_t count, int *i, char word[], int maxLen);
uint64_t readNumber (char line[], size_t count, int *i, bool skipSpaces);
uint64_t getDist (uint64_t holdTime, uint64_t raceTime);
bool isDigit (char c);
bool isLetter (char c);

int main (int argc, char* argv[])
{
    FILE *fp = NULL;
    char *buff = NULL;
    size_t buff_size = 128;
    size_t read;

    uint64_t times[64];
    uint64_t dists[64];
    char parseWord[32];

    t_pmap mapLookup[] = {
        { KEY_TIME, times },
        { KEY_DIST, dists }
    };

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-2") == 0)
            partTwo = true;
        else if (strcmp(argv[i], "-v") == 0)
            verbose = true;
        else if (fp == NULL && strstr(argv[i], ".txt") != NULL)
        {
            printf ("Using file: %s\n", argv[i]);
            fflush(stdout);
            fp = fopen (argv[i], "r");
        }
    }

    if (fp == NULL)
    {
        printf ("No file passed, trying input.txt in current folder.\n");
        fflush(stdout);
        fp = fopen ("input.txt", "r");
    }

    if (fp == NULL)
            exit (EXIT_FAILURE);

    while (read = getline(&buff, &buff_size, fp)) 
    {
        if (buff[0] == '\n')
        {
            if (feof(fp))
                break;
            continue;
        }
        buff[read - 1] = '\0';

        if (verbose)
            printf("Read %d chars -> '%s'\n", (int) read, buff);

        uint64_t *map;
        t_pmap *lookup;
        int charIndex = 0;
        readWord(buff, read, &charIndex, parseWord, 64);

        for (int i = 0; i < KEY_COUNT; i++)
        {
            lookup = mapLookup + i;
            
            if (strcmp(lookup->key, parseWord) == 0)
            {
                if (verbose)
                    printf("Got map: %d\n", i);
                map = lookup->map;
            }
        }

        uint64_t *entry = &map[0];
        (*entry) = 0;

        while (charIndex < read)
        {
            map[++(*entry)] = readNumber (buff, read, &charIndex, partTwo);
        }
        
        free(buff);
        buff = NULL;
    }

    uint64_t sum = 1;

    for (int i = 1; i < times[0]; i++)
    {
        uint64_t time = times[i];
        uint64_t record = dists[i];

        uint64_t mp = time / 2;
        bool isEven = time % 2 == 0;

        uint64_t dist = -1;
        uint64_t lastBad = 0;
        uint64_t lastGood = mp;
        uint64_t next = mp;
        uint64_t current = 0;
        bool found = false;

        while (!found)
        {
            current = next;
            dist = getDist(next, time);

            if (dist > record)
            {
                if (verbose)
                    printf("Good value: %lu, Next: ", current);
                lastGood = current;
                next -= (next - lastBad) / 2;
                if (verbose)
                    printf("%lu\n", next);
            }
            else
            {
                if (verbose)
                    printf("Bad value: %lu, Next: ", current);
                lastBad = current;
                next += (lastGood - next) / 2;
                if (verbose)
                    printf("%lu\n", next);
            }

            if (abs(lastGood - lastBad) < 2)
                found = true;
        }

        uint64_t diff = (mp - lastBad) * 2;
        if (isEven)
            diff -= 1;

        if (verbose)
            printf ("Got diff: %lu\n", diff);

        if (partTwo)
            sum = diff;
        else
            sum *= diff;
    }

    printf ("Result: %lu\n", sum);
}

uint64_t readNumber (char line[], size_t count, int *i, bool skipSpaces)
{
    char num[64] = {[0 ... 63] = '\0'};
    int ni = 0;

    char c = line[*i];
    for (; *i < count; c = line[++(*i)])
    {
        bool continueOnSpace = skipSpaces && c == ' ';

        if (!isDigit(c))
            if (ni == 0 || continueOnSpace)
                continue;
            else
                break;

        num[ni++] = c;
    }

    if (verbose)
        printf (" %s", num);

    return strtoul(num, NULL, 0);
}

void readWord (char line[], size_t count, int *i, char word[], int maxLen)
{
    int ci = 0;
    char c = line[*i];
    for (; *i < count && ci < maxLen - 1 && isLetter(c); c = line[++(*i)])
        word[ci++] = c;
    word[ci] = '\0';
    if (verbose)
        printf ("Read word: %s\n", word);
}

uint64_t getDist (uint64_t holdTime, uint64_t raceTime)
{
    return (holdTime * raceTime) - (holdTime * holdTime);
}

bool isDigit (char c)
{
    return c >= '0' && c <= '9';
}

bool isLetter (char c)
{
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}
