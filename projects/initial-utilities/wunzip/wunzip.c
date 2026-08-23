/*
Usage: wunzip [files...]
- Decode output of 'wzip'.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        int n;
        char ch;

        while (fread(&n, sizeof(int), 1, fp) == 1)
        {
            assert(fread(&ch, sizeof(char), 1, fp) == 1);

            // expand 1 run
            while (n-- > 0)
                printf("%c", ch);
        }

        fclose(fp);
    }

    return 0;
}