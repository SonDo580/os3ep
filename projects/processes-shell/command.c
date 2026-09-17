#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "command.h"

void init_command_array(CommandArray *commands)
{
    commands->commands = NULL;
    commands->capacity = 0;
    commands->count = 0;
}

static void init_arg_array(ArgArray *args)
{
    args->args = NULL;
    args->capacity = 0;
    args->count = 0;
}

void init_command(Command *command)
{
    command->out = NULL;
    init_arg_array(&command->args);
}

void reset_command_array(CommandArray *commands)
{
    // Free dynamically-allocated memory
    // - 'out', individual args point directly to input line -> don't free
    // - 'commands' itself, individual commands are local variables on stack -> don't free
    for (int i = 0; i < commands->count; i++)
    {
        ArgArray *args = &commands->commands[i].args;
        free(args->args);
    }
    free(commands->commands);

    // Reset state
    init_command_array(commands);
}

void push_command(CommandArray *commands, Command command)
{
    if (commands->count == commands->capacity)
    {
        commands->capacity = GROW_CAPACITY(commands->capacity);
        commands->commands = GROW_ARRAY(Command, commands->commands, commands->capacity);
    }
    commands->commands[commands->count] = command;
    commands->count++;
}

void push_arg(ArgArray *args, char *arg)
{
    if (args->count == args->capacity)
    {
        args->capacity = GROW_CAPACITY(args->capacity);
        args->args = GROW_ARRAY(char *, args->args, args->capacity);
    }
    args->args[args->count] = arg;
    args->count++;
}

void replace_arg(ArgArray *args, int index, char *arg)
{
    assert(index >= 0 && index < args->count);
    args->args[index] = arg;
}
