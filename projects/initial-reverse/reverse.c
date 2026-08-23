/*
reverse: Output lines in input in reverse order.

Usage: (default input/output is standard input/output)
./reverse
./reverse input.txt
./reverse input.txt output.txt
*/

#define _POSIX_C_SOURCE 200809L // enable POSIX.1-2008 features
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// linked list node
typedef struct ListNode
{
    char *s;
    struct ListNode *next;
} ListNode;

void free_list(ListNode *head)
{
    ListNode *curr = head;
    while (curr)
    {
        free(curr->s);
        ListNode *next = curr->next;
        free(curr);
        curr = next;
    }
}

int main(int argc, char **argv)
{
    if (argc > 3)
    { // too many arguments
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc == 3 && strcmp(argv[1], argv[2]) == 0)
    {
        fprintf(stderr, "reverse: input and output file must differ\n");
        exit(1);
    }

    // default input/output files
    FILE *in_fp = stdin;
    FILE *out_fp = stdout;

    // open input/output files if specified
    if (argc > 1)
    {
        char *in_filename = argv[1];
        in_fp = fopen(in_filename, "r");
        if (in_fp == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", in_filename);
            exit(1);
        }
    }
    if (argc == 3)
    {
        char *out_filename = argv[2];
        out_fp = fopen(out_filename, "w");
        if (out_fp == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", out_filename);
            fclose(in_fp);
            exit(1);
        }
    }

    // handle link (different paths pointing to the same inode)
    struct stat in_stat, out_stat;
    fstat(fileno(in_fp), &in_stat);
    fstat(fileno(out_fp), &out_stat);
    if (in_stat.st_ino == out_stat.st_ino)
    {
        fprintf(stderr, "reverse: input and output file must differ\n");
        fclose(in_fp);
        fclose(out_fp);
        exit(1);
    }

    ListNode *head = NULL;

    // collect input lines
    char *line = NULL;
    size_t linelen = 0;
    while (getline(&line, &linelen, in_fp) != -1)
    {
        // copy line and insert to head of linked-list
        ListNode *node = malloc(sizeof(ListNode));
        if (node == NULL)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        node->s = strdup(line);
        node->next = head;
        head = node;
    }

    // output lines in reverse order
    // -> traverse from head of linked list
    ListNode *curr = head;
    while (curr)
    {
        fprintf(out_fp, "%s", curr->s);
        curr = curr->next;
    }

    // free dynamically-allocated memory
    free(line);
    free_list(head);

    // close files
    if (in_fp != stdin)
        fclose(in_fp);
    if (out_fp != stdout)
        fclose(out_fp);

    return 0;
}