#ifndef __path_h__
#define __path_h__

typedef struct __PathArray
{
    char **paths;
    int capacity;
    int count;
} PathArray;

void init_path_array(PathArray *paths);
void add_default_paths(PathArray *paths);
void reset_path_array(PathArray *paths);
void push_path(PathArray *paths, char *path);

#endif