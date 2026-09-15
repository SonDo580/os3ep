#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#include "hashtable.h"
#include "exec.h"
#include "common.h"

void parse_instruction(char *instruction, char *tokens[], int *count)
{
  char *stringp = instruction;
  char *token;
  while ((token = strsep(&stringp, ",")) != NULL)
    tokens[(*count)++] = token;
}

void load_db(const char *db_filename, HTable *table)
{
  FILE *fp = Fopen(db_filename, "r");
  char *line = NULL;
  size_t linelen = 0;
  ssize_t nread = 0;

  while ((nread = getline(&line, &linelen, fp)) != -1)
  {
    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0'; // strip '\n'

    // Execute put command
    char *tokens[3]; // size = max_tokens (don't need dynamic array)
    tokens[0] = "p";
    int count = 1;
    parse_instruction(line, tokens, &count);
    exec_cmd(tokens, count, table);
  }

  free(line);
  fclose(fp);
}

void write_db(const char *db_filename, HTable *table)
{
  FILE *fp = Fopen(db_filename, "w");
  table_foreach(table, (ForEachCallback)write_entry, fp);
  fclose(fp);
}

int main(int argc, char **argv)
{
  if (argc == 1) // Do nothing
    return 0;

  // Create in-memory hash table
  HTable *table = table_create();

  // Load entries from database file
  const char *db_filename = "database.txt";
  load_db(db_filename, table);

  for (int i = 1; i < argc; i++)
  {
    // Parse instruction
    char *tokens[3]; // size = max_tokens (don't need dynamic array)
    int count = 0;
    parse_instruction(argv[i], tokens, &count);

    // Execute command
    exec_cmd(tokens, count, table);
  }

  // Persist data before exit
  write_db(db_filename, table);

  table_dealloc(table);
  return 0;
}