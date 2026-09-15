#ifndef __table_h__
#define __table_h__

#include <stddef.h>
#include <stdint.h>

typedef struct __HNode
{
    HNode *next;
    char *value;
    int64_t key;
} HNode;

typedef struct __HTable
{
    HNode **buckets; // array of linked-lists
    size_t num_buckets;
} HTable;

typedef void (*ForEachCallback)(HNode *);

void table_init(HTable *table, size_t num_buckets);
void table_upsert(HTable *table, int64_t key, char *value);
HNode **table_lookup(HTable *table, int64_t key);
int table_detach(HTable *table, int64_t key);
void table_clear(HTable *table);
void table_dealloc(HTable *table);
void table_foreach(HTable *table, ForEachCallback callback);

#endif
