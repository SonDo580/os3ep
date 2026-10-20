/*
Heap:
- Implement a complete binary tree with an array
  (the array represents the level-order traversal of the tree).
- Complete binary tree: all level except the last must be fully filled,
  and the last level is filled from left to right.
- A node at i has left child a 2*i + 1 and right child at 2*i + 2.
- A node at i (i != 0) has parent at (i - 1) // 2
- All nodes from n // 2 to n - 1 are leaves.

Min Heap:
- parent.val <= child.val
- min item is the root at index 0.
*/

#include <assert.h>

#include "conn_heap.h"
#include "common.h"

void conn_heap_init(ConnHeap *h, int size)
{
    assert(size > 0 && size <= MAX_QUEUE_SIZE);
    h->size = size;
    h->items = Malloc(h->size * sizeof(ConnHeapItem));
    h->count = 0;

    Pthread_mutex_init(&h->mutex, NULL);
    Pthread_cond_init(&h->fill, NULL);
    Pthread_cond_init(&h->empty, NULL);
}

void conn_heap_free(ConnHeap *h)
{
    free(h->items);
}

static inline int parent_index(int i)
{
    return (i + 1) / 2 - 1;
}

static inline int left_child_index(int i)
{
    return 2 * i + 1;
}

static inline int right_child_index(int i)
{
    return 2 * i + 2;
}

static void sift_up(ConnHeap *h, int i)
{
    ConnHeapItem item = h->items[i];
    while (i > 0 && h->items[parent_index(i)].filesize > item.filesize)
    {
        ConnHeapItem tmp = h->items[i];
        h->items[i] = h->items[parent_index(i)];
        h->items[parent_index(i)] = tmp;
        i = parent_index(i);
    }
}

static void sift_down(ConnHeap *h, int i)
{
    while (1)
    {
        int current = i;

        if (left_child_index(current) < h->count &&
            h->items[left_child_index(current)].filesize < h->items[current].filesize)
        {
            current = left_child_index(current);
        }

        if (right_child_index(current) < h->count &&
            h->items[right_child_index(current)].filesize < h->items[current].filesize)
        {
            current = right_child_index(current);
        }

        if (current == i)
            return;

        ConnHeapItem tmp = h->items[i];
        h->items[i] = h->items[current];
        h->items[current] = tmp;
        i = current;
    }
}

static void heap_push(ConnHeap *h, ConnHeapItem item)
{
    // append, sift up

    h->items[h->count] = item;
    h->count++;
    sift_up(h, h->count - 1);
}

static ConnHeapItem heap_pop(ConnHeap *h)
{
    // pop last item, swap with root, sift down
    // return old root

    assert(h->count > 0);
    ConnHeapItem minItem = h->items[0];
    ConnHeapItem lastItem = h->items[h->count - 1];
    h->count--;

    if (h->count > 0)
    {
        h->items[0] = lastItem;
        sift_down(h, 0);
    }

    return minItem;
}

void conn_heap_produce(ConnHeap *h, ConnHeapItem item)
{
    Pthread_mutex_lock(&h->mutex);
    while (h->count == h->size)
        Pthread_cond_wait(&h->empty, &h->mutex);

    heap_push(h, item);

    Pthread_cond_signal(&h->fill);
    Pthread_mutex_unlock(&h->mutex);
}

ConnHeapItem conn_heap_consume(ConnHeap *h)
{
    Pthread_mutex_lock(&h->mutex);
    while (h->count == 0)
        Pthread_cond_wait(&h->fill, &h->mutex);

    ConnHeapItem item = heap_pop(h);

    Pthread_cond_signal(&h->empty);
    Pthread_mutex_unlock(&h->mutex);

    return item;
}
