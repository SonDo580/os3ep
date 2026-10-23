/*
Recursive Search:

Write a program that prints out the names of each file and directory in the file system tree,
starting at a given point in the tree.

For example, when run without arguments,
the program should start with the current working directory and print its contents,
as well as the contents of any sub-directories, etc.,
until the entire tree, root at the CWD, is printed.

If given a single argument (of a directory name),
use that as the root of the tree instead.

Refine your recursive search with more fun options,
similar to the powerful 'find' command line tool.
*/

/*
Extensions:
- Restrict search depth.
- File name matching.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fnmatch.h>

void ls_recur(char *dir, char *pattern, int curr_depth, int max_depth)
{
    if (max_depth != -1 && curr_depth > max_depth)
        return;

    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        perror("opendir()");
        exit(EXIT_FAILURE);
    }

    errno = 0;

    struct dirent *ent;
    while ((ent = readdir(dp)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, ent->d_name);

        if (pattern == NULL || fnmatch(pattern, ent->d_name, 0) == 0)
            printf("%s\n", full_path);

        struct stat sbuf;
        if (stat(full_path, &sbuf) < 0)
        {
            perror("stat()");
            break;
        }

        if (S_ISDIR(sbuf.st_mode))
            ls_recur(full_path, pattern, curr_depth + 1, max_depth);
    }

    if (errno != 0)
        perror("readdir()");

    if (closedir(dp) < 0)
        perror("closedir()");
}

int main(int argc, char *argv[])
{
    char *usage_msg = "search [<base_dir>] [-name <pattern>] [-maxdepth <maxdepth>]\n";

    int max_depth = -1;   // no restriction
    char *pattern = NULL; // no filter
    char *base_dir = "."; // current working directory

    // Only set by arguments once
    int max_depth_set = 0;
    int pattern_set = 0;
    int base_dir_set = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-name") == 0)
        {
            if (!pattern_set && (i + 1 < argc))
            {
                pattern = argv[++i];
                pattern_set = 1;
            }
            else
            {
                fprintf(stderr, "%s", usage_msg);
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[i], "-maxdepth") == 0)
        {
            if (!max_depth_set && (i + 1 < argc))
            {
                max_depth = atoi(argv[++i]);
                if (max_depth < 0)
                {
                    fprintf(stderr, "max_depth must be >= 0\n");
                    exit(EXIT_FAILURE);
                }
                max_depth_set = 1;
            }
            else
            {
                fprintf(stderr, "%s", usage_msg);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (!base_dir_set)
            {
                base_dir = argv[i];
                base_dir_set = 1;
            }
            else
            {
                fprintf(stderr, "%s", usage_msg);
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("Base dir: %s\n", base_dir);
    printf("Max depth: %d\n", max_depth);
    printf("Pattern: %s\n", pattern ? pattern : "");

    ls_recur(base_dir, pattern, 0, max_depth);

    return 0;
}