/*
A simple hash table implementation:
- Collision resolution: separate chaining.
- Store key and value directly in hash table node.
- Hash function: abs(key) % num_buckets.
- Keep number of buckets fixed: num_buckets = pow(2, k).
  . Enable "micro optimization": x % pow(2, k) = x & (pow(2, k) - 1)
- Don't enforce maximum load factor (don't resize and redistribute entries).
*/

#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#include "hashtable.h"
#include "common.h"

HTable *table_create()
{
    HTable *table = Malloc(sizeof(HTable));
    table->num_buckets = 1 << 10;
    table->buckets = (HNode **)Calloc(table->num_buckets, sizeof(HNode *));
    table->num_entries = 0;
    return table;
}

static inline unsigned my_abs(int x)
{
    if (x == INT_MIN)
        return (unsigned)INT_MAX + 1U;
    return (x < 0) ? -x : x;
}

/* decide bucket based on hash code. */
static inline size_t table_index(HTable *table, int key)
{
    return my_abs(key) & (table->num_buckets - 1);
}

/* update entry value or insert at front of chain. */
void table_upsert(HTable *table, int key, char *value)
{
    HNode **from = table_lookup(table, key); // incoming pointer to target

    if (from == NULL)
    {
        // Insert at front of chain
        size_t index = table_index(table, key);
        HNode *new_node = (HNode *)Malloc(sizeof(HNode));
        new_node->next = table->buckets[index];
        new_node->value = value;
        new_node->key = key;
        table->buckets[index] = new_node;
        return;
    }

    // Update entry value
    HNode *target = *from;
    free(target->value);
    target->value = value;
}

/* return incoming pointer to target HNode*; return NULL if not found. */
HNode **table_lookup(HTable *table, int key)
{
    assert(table->buckets != NULL);
    size_t index = table_index(table, key);
    HNode **from = &table->buckets[index];

    HNode *curr;
    while ((curr = *from) != NULL)
    {
        if (curr->key == key)
            return from;
        from = &curr->next;
    }

    return NULL;
}

static void node_dealloc(HNode *node)
{
    free(node->value);
    free(node);
}

/* detach a node; return true on success, false if not found. */
bool table_detach(HTable *table, int key)
{
    HNode **from = table_lookup(table, key); // incoming pointer to target
    if (from == NULL)
        return false;

    // Detach target
    HNode *target = *from;
    *from = target->next;
    node_dealloc(target);
    return true;
}

/* deallocate all entries. */
void table_clear(HTable *table)
{
    for (int i = 0; i < table->num_buckets; i++)
    {
        HNode *curr = table->buckets[i];
        while (curr != NULL)
        {
            HNode *next = curr->next;
            node_dealloc(curr);
            curr = next;
        }
        table->buckets[i] = NULL;
    }
}

void table_dealloc(HTable *table)
{
    table_clear(table);
    free(table->buckets);
    table->buckets = NULL;
    table->num_buckets = 0;
}

/* invoke callback function for each node. */
void table_foreach(HTable *table, ForEachCallback callback, void* args)
{
    for (int i = 0; i < table->num_buckets; i++)
    {
        HNode *curr = table->buckets[i];
        while (curr != NULL)
        {
            callback(curr, args);
            curr = curr->next;
        }
    }
}