#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "path.h"

void init_path_array(PathArray *paths)
{
    paths->paths = NULL;
    paths->capacity = 0;
    paths->count = 0;
}

void add_default_paths(PathArray *paths)
{
    push_path(paths, "/bin");
}

void reset_path_array(PathArray *paths)
{
    // Free dynamically-allocated memory
    // - individual paths are created via strdup() -> must free.
    // - 'paths' itself is local variable on stack -> don't free.
    for (int i = 0; i < paths->count; i++)
        free(paths->paths[i]);
    free(paths->paths);

    // Reset state
    init_path_array(paths);
}

/* duplicate 'path' and add to 'paths'. */
void push_path(PathArray *paths, char *path)
{
    if (paths->count == paths->capacity)
    {
        paths->capacity = GROW_CAPACITY(paths->capacity);
        paths->paths = GROW_ARRAY(char *, paths->paths, paths->capacity);
    }
    paths->paths[paths->count] = strdup(path);
    paths->count++;
}
