#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "exec.h"

static bool parse_int(char *str, int *out)
{
    char *endptr;
    errno = 0; // reset before call
    long val = strtol(str, &endptr, 10);

    if (errno == ERANGE) // not within 'long' range
        return false;
    if (endptr == str) // no digits were parsed
        return false;
    if (*endptr != '\0') // has trailing non-digit characters
        return false;
    if (val > INT_MAX || val < INT_MIN) // not within 'int' range
        return false;

    *out = (int)val;
    return true;
}

static void exec_put(char **tokens, HTable *table)
{
    int key;
    if (!parse_int(tokens[1], &key))
    {
        fprintf(stderr, "invalid key %s\n", tokens[1]);
        return;
    }

    char *value = strdup(tokens[2]);
    table_upsert(table, key, value);
}

void write_entry(HNode *node, FILE *fp)
{
  fprintf(fp, "%d,%s\n", node->key, node->value);
}

static void exec_get(char **tokens, HTable *table)
{
    int key;
    if (!parse_int(tokens[1], &key))
    {
        fprintf(stderr, "invalid key %s\n", tokens[1]);
        return;
    }

    HNode **from = table_lookup(table, key);
    if (from == NULL)
        printf("%d not found\n", key);
    else
        write_entry(*from, stdout);
}

static void exec_delete(char **tokens, HTable *table)
{
    int key;
    if (!parse_int(tokens[1], &key))
    {
        fprintf(stderr, "invalid key %s\n", tokens[1]);
        return;
    }

    bool deleted = table_detach(table, key);
    if (!deleted)
        printf("%d not found\n", key);
}

static void exec_clear(HTable *table)
{
    table_clear(table);
}

static void exec_all(HTable *table)
{
    table_foreach(table, (ForEachCallback)write_entry, stdout);
}

void exec_cmd(char **tokens, int count, HTable *table)
{
    char *command = tokens[0];

    if (strcmp(command, "p") == 0)
    {
        assert(count == 3);
        exec_put(tokens, table);
    }
    else if (strcmp(command, "g") == 0)
    {
        assert(count == 2);
        exec_get(tokens, table);
    }
    else if (strcmp(command, "d") == 0)
    {
        assert(count == 2);
        exec_delete(tokens, table);
    }
    else if (strcmp(command, "a") == 0)
    {
        assert(count == 1);
        exec_all(table);
    }
    else if (strcmp(command, "c") == 0)
    {
        assert(count == 1);
        exec_clear(table);
    }
    else
    {
        fprintf(stderr, "bad command\n");
    }
}