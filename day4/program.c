#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool verbose = false;
bool partTwo = false;

int procLn (char line[], size_t count, bool curNums[], int reval[], int ln);
int readNumber (char line[], size_t count, int *i);
int sumScratchers (char line[], size_t count, int *i, bool curNums[], int reval[], int ln);
void readWinners (char line[], size_t count, int *i, bool curNums[]);
bool isDigit (char c);

int main (int argc, char* argv[])
{
    FILE *fp = NULL;
    char *buff = NULL;
    size_t buff_size = 128;

    size_t read;

    bool curNums[100] = {false};
    int reval[200] = {[0 ... 199] = 1};
    int line = 0;

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

    int _sum = 0;
    while (read = getline(&buff, &buff_size, fp)) 
    {
        if (read <= 1) break;
        buff[read - 1] = '\0';
        if (verbose)
            printf("Read %d chars -> '%s'\n", (int) read, buff);
        fflush(stdout);
        _sum += procLn (buff, read, curNums, reval, line);
        if (verbose)
            printf("Total = %d\n", _sum);
        for (int i = 0; i < 100; i++)
            curNums[i] = false;
        line++;
        free(buff);
        buff = NULL;
    }
    if (!verbose)
        printf("Total = %d\n", _sum);
    fclose(fp);
}

int procLn (char line[], size_t count, bool curNums[], int reval[], int ln)
{
    int i = 0;
    readWinners (line, count, &i, curNums);
    return sumScratchers (line, count, &i, curNums, reval, ln);
}

int sumScratchers (char line[], size_t count, int *i, bool curNums[], int reval[], int ln)
{
    if (verbose)
        printf ("Reading scratchers: ");
    int sum = 0;
    char c = line[*i];
    for (; *i < count && c != '\n'; c = line[(*i)++])
    {
        int cur = readNumber (line, count, i);
        if (curNums[cur])
        {
            if (partTwo)
                sum++;
            else
            {
                if (sum == 0)
                    sum = 1;
                else sum *= 2;
            }
        }
    }
    if (verbose)
        printf ("\n");
    if (partTwo)
    {
        for (int i = 0; i < sum && i + 1 < 200; i++)
            reval[ln + i + 1] += reval[ln];
        return reval[ln];
    }
    return sum;
}

void readWinners (char line[], size_t count, int *i, bool curNums[])
{
    if (verbose)
        printf ("Reading winners: ");
    char c = line[*i];
    for (; *i < count && c != ':'; c = line[(*i)++]);
    for (; *i < count && c != '|'; c = line[(*i)++])
    {
        int num = readNumber (line, count, i);
        curNums[num] = true;
    }
    if (verbose)
        printf ("\n");
}

int readNumber (char line[], size_t count, int *i)
{
    char num[4] = {'\0'};
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

    return atoi(num);
}

bool isDigit (char c)
{
    return c >= '0' && c <= '9';
}

