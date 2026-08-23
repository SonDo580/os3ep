/*
Usage: wzip [files...]
- Apply run-length encoding:
  . n same-type characters in a row = number n + 1 character instance
  . Example: "aaa" -> "3a"
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    // handle runs across files
    int curr_len = 0;
    int prev_ch = -1;
    int ch;

    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("wzip: cannot open file\n");
            exit(1);
        }

        while ((ch = fgetc(fp)) != EOF)
        {
            if (curr_len == 0 || ch == prev_ch)
                curr_len++; // continue current run
            else
            {
                fwrite(&curr_len, sizeof(int), 1, stdout);
                char c = (char)prev_ch;
                fwrite(&c, sizeof(char), 1, stdout);

                curr_len = 1; // start new run
            }

            prev_ch = ch;
        }

        fclose(fp);
    }

    if (curr_len > 0) // handle last run
    {
        fwrite(&curr_len, sizeof(int), 1, stdout);
        char c = (char)prev_ch;
        fwrite(&c, sizeof(char), 1, stdout);
    }

    return 0;
}