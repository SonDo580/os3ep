/*
kv: a simple persistent key-value store.

Usage:
> ./kv    (do nothing)
> ./kv command1 <command2 ...>    (execute commands)

Supported commands:
- put: p,key,value (key is an integer, value is a string without ',')
  . update entry value or insert entry.
- get: g,key
  . found -> print '<key>,<value>'.
  . not found -> print '<key> not found'.
- delete: d,key
  . found -> delete entry and print nothing.
  . not found -> print '<key> not found'.
- clear: c
  . removes all entries.
- all: a
  . print all entries in any order, with 1 entry per line.

Persistence:
- Use a single file 'database.txt' to store all entries in plain-text.
- 'kv' reads all the file into memory at startup, into a hash table.
- 'kv' updates the in-memory data structure when processing command.
- 'kv' writes all entries to the database file before exiting.
- More sophisticated techniques are needed to handle very large database
  and tolerate crashes.
*/

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "hashtable.h"
#include "exec.h"

const char *db_filename = "database.txt";

int main(int argc, char **argv)
{
  if (argc == 1) // Do nothing
    return 0;

  // Create hash table
  HTable *table = table_create();
  // TODO: load entries from file

  for (int i = 1; i < argc; i++)
  {
    // Parse instruction
    char *stringp = argv[i];
    char *tokens[3]; // size = max_tokens (don't need dynamic array)
    char *token;
    int count = 0;
    while ((token = strsep(&stringp, ",")) != NULL)
      tokens[count++] = token;

    // Execute command
    exec_cmd(tokens, count, table);
  }

  // TODO: persistent

  table_dealloc(table);
  return 0;
}