/*
Usage: wcat [files...]
- print out each file in turn.
*/

#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1024 // max number of characters in 1 line

int main(int argc, char **argv)
{
    char line[MAXLINE];
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        while (fgets(line, MAXLINE, fp))
            printf("%s", line);

        fclose(fp);
    }

    return 0;
}