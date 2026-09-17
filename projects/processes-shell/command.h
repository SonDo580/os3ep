#ifndef __command_h__
#define __command_h__

typedef struct __ArgArray
{
    char **args;
    int capacity;
    int count;
} ArgArray;

typedef struct __Command
{
    ArgArray args;
    char *out;
} Command;

typedef struct __CommandArray
{
    Command *commands;
    int capacity;
    int count;
} CommandArray;

void init_command_array(CommandArray *commands);
void init_command(Command *command);
void reset_command_array(CommandArray *commands);
void push_command(CommandArray *commands, Command command);
void push_arg(ArgArray *args, char *arg);
void replace_arg(ArgArray *args, int index, char *arg);

#endif