#include <stdlib.h>

typedef struct
{
    int count;
    int capacity;
    int *entries;
} Vector;

void append(Vector *vector, int x)
{
    if (vector->count == vector->capacity)
    {
        vector->capacity *= 2;
        if (vector->capacity == 0)
            vector->capacity = 8;
        vector->entries = realloc(vector->entries,
                                  sizeof(int) * vector->capacity);
    }
    vector->entries[vector->count++] = x;
}

Vector *initVector()
{
    Vector *vector = malloc(sizeof(Vector));
    vector->capacity = 0;
    vector->count = 0;
    vector->entries = NULL;
    return vector;
}

void freeVector(Vector *vector)
{
    free(vector->entries);
    free(vector);
}

int main()
{
    Vector *vector = initVector();
    for (int i = 0; i < 20; i++)
    {
        append(vector, i);
    }
    freeVector(vector);
    return 0;
}