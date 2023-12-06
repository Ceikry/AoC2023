#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define DEST_OFFSET 0
#define SOURCE_OFFSET 1
#define DIFF_OFFSET 2
#define MAP_SIZE 150
#define SEED_MAX 50

#define KEY_COUNT 8
#define KEY_SEEDS "seeds"
#define KEY_SOILMAP "seed"
#define KEY_FERTMAP "soil"
#define KEY_WATERMAP "fertilizer"
#define KEY_LIGHTMAP "water"
#define KEY_TEMPMAP "light"
#define KEY_HUMIDMAP "temperature"
#define KEY_LOCMAP "humidity"

enum ParseMode {
    SEEDS,
    SOILMAP,
    FERTMAP,
    WATERMAP,
    LIGHTMAP,
    TEMPMAP,
    HUMIDMAP,
    LOCMAP
};

typedef struct { char *mapKey; enum ParseMode mode; } t_pmode;
typedef struct { enum ParseMode mode; unsigned long int* map; } t_pmap;

t_pmode modeLookup[] = {
    { KEY_SEEDS,    SEEDS    },
    { KEY_SOILMAP,  SOILMAP  },
    { KEY_FERTMAP,  FERTMAP  },
    { KEY_WATERMAP, WATERMAP },
    { KEY_LIGHTMAP, LIGHTMAP },
    { KEY_TEMPMAP,  TEMPMAP  },
    { KEY_HUMIDMAP, HUMIDMAP },
    { KEY_LOCMAP,   LOCMAP   }
};

bool verbose = false;
bool partTwo = false;

void procMapRange 
(
        unsigned long int *map, 
        unsigned long int key, 
        unsigned long int keyRange, 
        unsigned long int *results
);
unsigned long int procMap (unsigned long int *map, unsigned long int key);
unsigned long int readNumber (char line[], size_t count, int *i);
void readWord (char line[], size_t count, int *i, char word[], int maxLen);
bool isDigit (char c);
bool isLetter (char c);

int main (int argc, char* argv[])
{
    FILE *fp = NULL;
    char *buff = NULL;
    size_t buff_size = 128;
    size_t read;

    unsigned long int seed    [SEED_MAX] = { 0, [1 ... (SEED_MAX - 1)] = -1};
    unsigned long int soilMap [MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int fertMap [MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int waterMap[MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int lightMap[MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int tempMap [MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int humidMap[MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};
    unsigned long int locMap  [MAP_SIZE] = {[0 ... (MAP_SIZE - 1)] =  0};

    t_pmap mapLookup[] = {
        { SEEDS, seed },
        { SOILMAP, soilMap },
        { FERTMAP, fertMap },
        { WATERMAP, waterMap },
        { LIGHTMAP, lightMap },
        { TEMPMAP, tempMap },
        { HUMIDMAP, humidMap },
        { LOCMAP, locMap }
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


    enum ParseMode mode = SEEDS;
    char modeWord[20] = {[0 ... 19] = '\0'};
    t_pmode *pmode = NULL;
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

        int ci = 0;
        if (!isDigit(buff[0]))
        {
            readWord (buff, read, &ci, modeWord, 20);

            for (int i = 0; i < KEY_COUNT; i++)
            {
                pmode = modeLookup + i;
                if (strcmp(pmode->mapKey, modeWord) == 0)
                {
                    mode = pmode->mode;
                    if (verbose)
                        printf ("Found mode: %s - %d\n", modeWord, pmode->mode);
                    break;
                }
            }

            if (mode != SEEDS)
                continue;
        }
      
        t_pmap *curMap = NULL;
        for (int i = 0; i < KEY_COUNT; i++)
        {
            curMap = mapLookup + i;
            if (curMap->mode == mode)
                break;
            curMap = NULL;
        }

        if (verbose)
            printf ("Found map from mode: %d\n", mode);

        if (curMap == NULL)
            continue;

        if (verbose)
            printf ("Found numbers:");
        for (; ci < read; ci++)
        {
            if (!isDigit(buff[ci]))
                continue;

            unsigned long int *entry = &curMap->map[0];
            fflush(stdout);
            curMap->map[++(*entry)] = readNumber (buff, read, &ci);
        }
        if (verbose)
            printf ("\n");
        
        free(buff);
        buff = NULL;
    }

    unsigned long int lowestLoc = ULONG_MAX;
    
    if (partTwo)
    {
        unsigned long int soil      [2048];
        unsigned long int fert      [2048];
        unsigned long int water     [2048];
        unsigned long int light     [2048];
        unsigned long int temp      [2048];
        unsigned long int humidity  [2048];
        unsigned long int loc       [2048];

        for (int si = 1; si < seed[0]; si += 2)
        {
            soil    [0] = 0;
            fert    [0] = 0;
            water   [0] = 0;
            light   [0] = 0;
            temp    [0] = 0;
            humidity[0] = 0;
            loc     [0] = 0;

            unsigned long int seedNum;
            unsigned long int seedRange;

            seedNum = seed[si];
            seedRange = seed[si + 1];

            if (seedNum == -1)
                break;

            procMapRange(soilMap, seedNum, seedRange, soil);
            for (int i = 1; i < soil[0]; i += 2)
                procMapRange(fertMap, soil[i], soil[i + 1], fert);
            for (int i = 1; i < fert[0]; i += 2)
                procMapRange(waterMap, fert[i], fert[i + 1], water);
            for (int i = 1; i < water[0]; i += 2)
                procMapRange(lightMap, water[i], water[i + 1], light);
            for (int i = 1; i < light[0]; i += 2)
                procMapRange(tempMap, light[i], light[i + 1], temp);
            for (int i = 1; i < temp[0]; i += 2)
                procMapRange(humidMap, temp[i], temp[i + 1], humidity);
            for (int i = 1; i < humidity[0]; i += 2)
                procMapRange(locMap, humidity[i], humidity[i + 1], loc);

            for (int i = 1; i < loc[0]; i += 2)
            {
                if (loc[i] < lowestLoc)
                {
                        printf ("Found new lowest loc: %lu\n", loc[i]);
                    lowestLoc = loc[i];
                }
            }
        }
    }
    else
    {
        for (int si = 1; si < seed[0]; si++)
        {
            unsigned long int seedNum;
            unsigned long int soil;
            unsigned long int fert;
            unsigned long int water;
            unsigned long int light;
            unsigned long int temp;
            unsigned long int humidity;
            seedNum = seed[si];
            if (seedNum == -1)
                break;

            soil = procMap(soilMap, seedNum);
            fert = procMap(fertMap, soil);
            water = procMap(waterMap, fert);
            light = procMap(lightMap, water);
            temp = procMap(tempMap, light);
            humidity = procMap(humidMap, temp);

            unsigned long int loc = procMap(locMap, humidity);
            if (loc == -1)
                continue;
            if (loc < lowestLoc)
            {
                    printf ("Got new lowest loc: %lu\n", loc);
                lowestLoc = loc;
            }
        }
    }

    printf("Closest = %lu\n", lowestLoc);

    fclose(fp);
}

void procMapRange 
(
        unsigned long int *map, 
        unsigned long int keyStart, 
        unsigned long int keyRange, 
        unsigned long int *results
)
{
    unsigned long int destStart;
    unsigned long int sourceStart;
    unsigned long int diff;
    unsigned long int sourceEnd;
    unsigned long int keyEnd = keyStart + keyRange - 1;

    if (keyRange == -1)
        return;

    if (verbose)
        printf ("Checking keyStart %lu with range %lu\n", keyStart, keyRange);

    unsigned long int* entry = &results[0];
    for (int i = 1; i < *(map); i += 3)
    {
        destStart = *(map + DEST_OFFSET + i);
        sourceStart = *(map + SOURCE_OFFSET + i);
        diff = *(map + DIFF_OFFSET + i);
        sourceEnd = sourceStart + diff - 1;

        if (keyStart >= sourceStart && keyStart < sourceEnd) // if start < nStart < end
        {
            unsigned long int keyOffset = keyStart - sourceStart;
            //the + 1 here accounts for the start-inclusive behavior of the range parameter.
            unsigned long int keyDiff = (keyEnd < sourceEnd ? keyEnd - keyStart : sourceEnd - keyStart) + 1;
            
            results[++(*entry)] = destStart + keyOffset;
            results[++(*entry)] = keyDiff >= 0 ? keyDiff : 0;
            if (verbose)
                printf ("Adding result: %lu - %lu\n", destStart + keyOffset, keyDiff);

            if (keyEnd > sourceEnd) // if start < nStart < end < nEnd
            {
                unsigned long int remStart = sourceEnd;
                unsigned long int remDiff = keyEnd - sourceEnd;
                if (verbose)
                    printf ("Reproc with: %lu, %lu\n", remStart, remDiff);
                procMapRange(map, remStart, remDiff, results);
            }
            return;
        }
        else if (keyStart < sourceStart && keyEnd >= sourceStart && keyEnd <= sourceEnd) // if start < nEnd < end 
        {
            unsigned long int keyDiff = keyEnd - sourceStart;
            results[++(*entry)] = destStart;
            results[++(*entry)] = keyDiff;
            if (verbose)
                printf ("Adding result: %lu - %lu\n", destStart, keyDiff);

            unsigned long int remDiff = sourceStart - keyStart;
            if (verbose)
                printf ("Reproc with: %lu, %lu\n", keyStart, remDiff);
            procMapRange(map, keyStart, remDiff, results);
            return;
        }
        else if (keyStart < sourceStart && keyEnd > sourceEnd) // if nStart < start < end < nEnd
        {
            unsigned long int remADiff = sourceStart - keyStart;
            unsigned long int remBDiff = keyEnd - sourceEnd;

            results[++(*entry)] = sourceStart;
            results[++(*entry)] = diff;
            if (verbose)
                printf ("Adding result: %lu - %lu\n", sourceStart, diff);

            if (verbose)
                printf ("Reproc with: %lu, %lu\n", keyStart, remADiff);
            procMapRange(map, keyStart, remADiff, results);
            if (verbose)
                printf ("Reproc with: %lu, %lu\n", sourceEnd, remBDiff);
            procMapRange(map, sourceEnd, remBDiff, results); 
            return;
        }
    }

    results[++(*entry)] = keyStart;
    results[++(*entry)] = keyRange;
}

unsigned long int procMap (unsigned long int *map, unsigned long int key)
{
    unsigned long int destStart;
    unsigned long int sourceStart;
    unsigned long int diff;

    if (verbose)
        printf ("Checking key: %lu\n", key);

    for (int i = 1; i < *(map); i += 3)
    {
        destStart = *(map + DEST_OFFSET + i);
        sourceStart = *(map + SOURCE_OFFSET + i);
        diff = *(map + DIFF_OFFSET + i);

        if (key >= sourceStart && key <= sourceStart + diff - 1)
        {
            return destStart + (key - sourceStart);
        }
    }

    return key;
}

unsigned long int readNumber (char line[], size_t count, int *i)
{
    char num[50] = {'\0'};
    int ni = 0;

    char c = line[*i];
    for (; *i < count; c = line[++(*i)])
    {
        if (c == ' ')
            if (ni == 0)
                continue;
            else
                break;

        if (!isDigit(c))
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

bool isDigit (char c)
{
    return c >= '0' && c <= '9';
}

bool isLetter (char c)
{
    return c >= 'a' && c <= 'z';
}
