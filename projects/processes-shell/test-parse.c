#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "command.h"
#include "parse.h"

bool optional_str_eq(char *s1, char *s2)
{
    if (s1 == NULL)
        return s2 == NULL;
    if (s2 == NULL)
        return s1 == NULL;
    return strcmp(s1, s2) == 0;
}

bool args_eq(ArgArray *args, ArgArray *expected_args)
{
    if (args->count != expected_args->count)
        return false;
    for (int i = 0; i < args->count; i++)
        if (!optional_str_eq(args->args[i], expected_args->args[i]))
            return false;
    return true;
}

bool command_eq(Command *command, Command *expected_command)
{

    return optional_str_eq(command->out, expected_command->out) &&
           args_eq(&command->args, &expected_command->args);
}

bool commands_eq(CommandArray *commands, CommandArray *expected_commands)
{
    if (commands->count != expected_commands->count)
        return false;
    for (int i = 0; i < commands->count; i++)
        if (!command_eq(&commands->commands[i], &expected_commands->commands[i]))
            return false;
    return true;
}

void test_parse(
    const char *line, CommandArray *expected_commands, bool expected_parse_success)
{
    printf("Start: %s\n", line);

    CommandArray commands;
    init_command_array(&commands);

    char *copied_line = strdup(line);
    bool parse_success = parse_commands(copied_line, &commands);
    assert(parse_success == expected_parse_success);

    if (!expected_parse_success)
    {
        printf("Done: %s\n", line);
        return;
    }

    if (!commands_eq(&commands, expected_commands))
        exit(EXIT_FAILURE);

    free(copied_line);
    reset_command_array(&commands);

    printf("Done: %s\n", line);
}

int main()
{
    char *line;
    CommandArray expected_commands;

    // success: empty command
    line = "";
    expected_commands = (CommandArray){.commands = NULL, .count = 0};
    test_parse("", &expected_commands, true);

    // success: single only-spaces command
    line = " \t\t \n";
    expected_commands = (CommandArray){.commands = NULL, .count = 0};
    test_parse(line, &expected_commands, true);

    // success: single command
    line = "wc wish.c";
    expected_commands = (CommandArray){
        .commands = (Command[]){
            {.args = {.args = (char *[]){"wc", "wish.c"}, .count = 2},
             .out = NULL}},
        .count = 1};
    test_parse(line, &expected_commands, true);

    // success: single command + arbitrary spaces
    line = " wc \t\t wish.c \n";
    expected_commands = (CommandArray){
        .commands = (Command[]){
            {.args = {.args = (char *[]){"wc", "wish.c"}, .count = 2},
             .out = NULL}},
        .count = 1};
    test_parse(line, &expected_commands, true);

    // success: parallel commands + output redirection
    line = "ls -la . > out.txt & wc wish.c";
    expected_commands = (CommandArray){
        .commands = (Command[]){
            {.args = {.args = (char *[]){"ls", "-la", "."}, .count = 3},
             .out = "out.txt"},
            {.args = {.args = (char *[]){"wc", "wish.c"}, .count = 2},
             .out = NULL}},
        .count = 2};
    test_parse(line, &expected_commands, true);

    // success: parallel commands + output redirection + arbitrary spaces
    line = " ls \t\t -la .>out.txt&wc \t wish.c\n";
    expected_commands = (CommandArray){
        .commands = (Command[]){
            {.args = {.args = (char *[]){"ls", "-la", "."}, .count = 3},
             .out = "out.txt"},
            {.args = {.args = (char *[]){"wc", "wish.c"}, .count = 2},
             .out = NULL}},
        .count = 2};
    test_parse(line, &expected_commands, true);

    // failed: no command before ">"
    line = " >output.txt";
    test_parse(line, NULL, false);

    // failed: no filename after ">"
    line = "ls -la . > ";
    test_parse(line, NULL, false);

    // failed: leading '&'
    line = " & ls .";
    test_parse(line, NULL, false);

    // failed: trailing '&'
    line = "ls . & ";
    test_parse(line, NULL, false);

    // failed: consecutive '&'
    line = "ls . & & wc wish.c";
    test_parse(line, NULL, false);

    // ... more test cases
}