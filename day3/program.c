#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PARSEBUG   -1
#define SIZE_LINE   141

bool verbose  = false;
bool partTwo  = false;
bool firstSet = true;

int procLn (char *lines[], bool first);
bool isDigit (char c);
bool isSym (char c);
int getNum (char line[], int index);
int getNumAt (char *line[], int x, int y);

int main (int argc, char* argv[])
{
    FILE *fp;
    char* buff[3] = {NULL, NULL, NULL};
    size_t buff_size = (size_t) SIZE_LINE;

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
    int _lineNum = 0;
    bool shouldBreak = false;
    while (read = getline(&buff[_lineNum++], &buff_size, fp)) 
    {
        if (read <= 1) break;

        if (feof(fp)) 
        {
            printf ("Read EOF, NULLING remaining lines. Start: %d\n", --_lineNum);
            for (; _lineNum < 3; _lineNum++)
                buff[_lineNum] = NULL;
            shouldBreak = true;
        }
        else
        {
            buff[_lineNum - 1][read - 1] = '\0';
        }

        if (_lineNum == 3) 
        {
            if (verbose)
                printf("Got lines:\n%s\n%s\n%s\n", buff[0], buff[1], buff[2]);

            _sum += procLn (buff, firstSet);

            if (partTwo)
            {
                free(buff[0]);
                buff[0] = buff[1];
                buff[1] = buff[2];
                buff[2] = NULL;
                _lineNum = 2;
            }
            else
            {
                free(buff[0]);
                free(buff[1]);
                buff[0] = buff[2];
                buff[1] = buff[2] = NULL;
                _lineNum = 1;
            }

            firstSet = false;

            if (shouldBreak)
                break;
        }
    }
    printf("Total = %d\n", _sum);
}

bool isDigit (char c) 
{
    return c >= '0' && c <= '9';
}

bool isSym (char c) 
{
    if (partTwo)
        return c == '*';

    return(    (c >= '!' && c <= '/') 
            || (c >= ':' && c <= '@') 
            || (c >= '[' && c <= '_') 
            || (c >= '{' && c <= '~'))
        && c != '.';
}

int getNum (char line[], int index)
{
    int start = index;
    char num[4] = {[0 ... 2] = ' ', '\0'};
    int ni = 0;

    char c = line[start];
    for (; !isSym(c) && c != '.' && start >= 0; c = line[--start]);
    for (c = line[++start]; isDigit(c) && start < SIZE_LINE; c = line[++start])
    {
        num[ni++] = c;
        
        if (!partTwo)
            line[start] = '.';
    }

    if (verbose)
        printf (" %s", num);

    return atoi(num);
}

int procLn (char *lines[], bool first) 
{
    int symbolIndices[3][32] = {{0}};
    int symIndex[3] = {0};
    int sum = 0;

    for (int ln = 0; ln < 3; ln++)
    {
        if (lines[ln] == NULL) break;

        if (verbose)
            printf("Line %d, Found Symbols:", ln + 1);
        for (int ci = 0; ci < SIZE_LINE; ci++) 
        {
            if (isSym(lines[ln][ci]))
            {
                if (verbose)
                    printf(" %c", lines[ln][ci]);
                symbolIndices[ln][symIndex[ln]++] = ci;
            }
        }
        if (verbose)
            printf("\n");
    }
    
    for (int ln = 0; ln < 3; ln++)
    {
        if (verbose)
            printf ("Line %d:\n", ln + 1);
        for (int i = 0; i < symIndex[ln]; i++)
        {
            int symIndex = symbolIndices[ln][i];
            int foundNums[8] = {0};
            int ni = 0;

            if (verbose)
                printf ("symindex: %03d -> ", symIndex);
            
            if (verbose)
                printf ("%c nums:", lines[ln][symIndex]);
            
            int _lastNum = 0;
            for (int i = 0; i < 9; i++) 
            {
                int xMod = (i % 3) - 1;
                int yMod = (i / 3) - 1;

                if (xMod == 0 && yMod == 0)
                    continue;

                if (ln == 0 && !first && yMod == 0)
                    continue;

                int num = getNumAt(lines, symIndex + xMod, ln + yMod);
                if (partTwo && num == _lastNum)
                    continue;

                if (num != -1)
                    foundNums[ni++] = num;
                _lastNum = num;
            }

            if (partTwo && ni != 2)
            {
                if (verbose)
                    printf ("\n");
               continue;
            }
            else if (partTwo)
                lines[ln][symIndex] = '.';

            int lineSum = 0;
            for (int i = 0; i < ni; i++) 
            {
                if (foundNums[i] == -1) continue;

                if (partTwo)
                {
                    if (lineSum == 0)
                        lineSum = foundNums[i];
                    else
                        lineSum *= foundNums[i];
                }
                else
                    lineSum += foundNums[i];
            }
            sum += lineSum;

            if (verbose)
                printf ("\n");
        }
        if (verbose)
            printf ("\n");
    }

    if (verbose)
        printf ("Section Total = %d\n", sum);
    return sum;
}

int getNumAt (char *lines[], int x, int y) {
    if (y >= 3 || y < 0 || x < 0 || x >= SIZE_LINE) 
        return -1;
    if (lines[y] == NULL) return -1;

    if (isDigit(lines[y][x]))
        return getNum (lines[y], x);

    return -1;
}
