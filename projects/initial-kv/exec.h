#ifndef __exec_h__
#define __exec_h__

#include "hashtable.h"

void exec_cmd(char **tokens, int count, HTable *table);
void write_entry(HNode *node, FILE *fp);

#endif