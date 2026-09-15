#ifndef __table_h__
#define __table_h__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct __HNode
{
    struct __HNode *next;
    char *value;
    int key;
} HNode;

typedef struct __HTable
{
    HNode **buckets;    // array of linked-lists
    size_t num_buckets; // fixed, arbitrary power of 2
    size_t num_entries;
} HTable;

typedef void (*ForEachCallback)(HNode *);

HTable *table_create();
void table_upsert(HTable *table, int key, char *value);
HNode **table_lookup(HTable *table, int key);
bool table_detach(HTable *table, int key);
void table_clear(HTable *table);
void table_dealloc(HTable *table);
void table_foreach(HTable *table, ForEachCallback callback);

#endif
