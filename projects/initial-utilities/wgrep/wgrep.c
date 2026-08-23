/*
Usage: wgrep <keyword> [files...]
- For each file, print out the lines with user-specified search term.
*/

#define _POSIX_C_SOURCE 200809L // enable POSIX.1-2008 features
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_file(const char *keyword, FILE *fp)
{
    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, fp) != -1)
    {
        if (strstr(line, keyword))
            printf("%s", line);
    }
    free(line);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    const char *keyword = argv[1];
    if (strlen(keyword) == 0) // match no lines
        return 0;

    if (argc == 2)
        handle_file(keyword, stdin);
    else
    {
        for (int i = 2; i < argc; i++)
        {
            char *filename = argv[i];
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("wgrep: cannot open file\n");
                exit(1);
            }

            handle_file(keyword, fp);

            fclose(fp);
        }
    }

    return 0;
}