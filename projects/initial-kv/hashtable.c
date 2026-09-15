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

#include "hashtable.h"

void table_init(HTable *table, size_t num_buckets)
{
    // Ensure num_buckets is power of 2
    assert(num_buckets > 0 && (num_buckets & (num_buckets - 1)) == 0);

    table->buckets = (HNode **)calloc(num_buckets, sizeof(HNode *));
    table->num_buckets = num_buckets;
}

/* decide bucket based on hash code. */
static inline size_t table_index(HTable *table, int64_t key)
{
    return abs(key) & (table->num_buckets - 1);
}

/* update entry value or insert at front of chain. */
void table_upsert(HTable *table, int64_t key, char *value)
{
    HNode **from = table_lookup(table, key); // incoming pointer to target

    if (from == NULL)
    {
        // Insert at front of chain
        size_t index = table_index(table, key);
        HNode *new_node = (HNode *)malloc(sizeof(HNode));
        new_node->next = table->buckets[index];
        new_node->value = value;
        new_node->key = key;
        return;
    }

    // Update entry value
    HNode *target = *from;
    free(target->value);
    target->value = value;
}

static inline uint64_t abs(int64_t x)
{
    return (x < 0) ? -x : x;
}

/* return incoming pointer to target HNode*; return NULL if not found. */
HNode **table_lookup(HTable *table, int64_t key)
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

/* detach a node; return 1 on success, 0 if not found. */
int table_detach(HTable *table, int64_t key)
{
    HNode **from = table_lookup(table, key); // incoming pointer to target
    if (from == NULL)
        return 0;

    // Detach target
    HNode *target = *from;
    *from = target->next;
    node_dealloc(target);
    return 1;
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
void table_foreach(HTable *table, ForEachCallback callback)
{
    for (int i = 0; i < table->num_buckets; i++)
    {
        HNode *curr = table->buckets[i];
        while (curr != NULL)
        {
            callback(curr);
            curr = curr->next;
        }
    }
}