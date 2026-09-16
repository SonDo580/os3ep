#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "command.h"

const char *ampersand_op = "&";
const char *redirect_op = ">";
const char *spaces = " \t\n";

/* Parse commands from input line.
Return true on success, false on failure.

- Example: `ls -la . > out.txt & wc wish.c`
- Parsed:
```json
[
  {args=["ls", "-la", ".", NULL], out="out.txt"},
  {args=["wc, "wish.c", NULL], out=NULL},
]
```
*/
bool parse_commands(char *line, CommandArray *commands)
{
    char *command_str;
    bool encountered_only_spaces_command = false;

    while ((command_str = strsep(line, ampersand_op)) != NULL)
    {
        if (encountered_only_spaces_command)
        { // parallel commands must not have any only-spaces (<-> empty) command
            return false;
        }

        if (*command_str == '\0')
        { // leading/trailing "&" or consecutive "&&"
            return false;
        }

        Command command;
        init_command(&command);

        // Parse arguments
        // args_str: full command_str or before ">"
        char *args_str = strsep(command_str, redirect_op);
        assert(args_str != NULL); // since command_str != ""
        if (*args_str == '\0')
        { // leading ">"
            return false;
        }

        char *arg;
        while (arg = strtok(args_str, spaces) != NULL)
            push_arg(&command.args, arg);

        if (command.args.count == 0)
        { // args_str contains only whitespaces
            if (command_str != NULL)
            { // args_str is followed by ">" (doesn't match full command_str)
                return false;
            }
            else if (commands->count > 0)
            { // parallel commands must not have any only-spaces (<-> empty) command
                return false;
            }
            else
            { // allow skipping single only-spaces (<-> empty) comand, fail if detect next command
                encountered_only_spaces_command = true;
                continue;
            }
        }

        // Parse redirect output
        // out_str: empty or after ">"
        char *out_str = strsep(command_str, redirect_op);
        if (out_str != NULL)
        {
            if (*out_str == '\0')
            { // trailing ">"
                return false;
            }

            command.out = strtok(out_str, spaces);
            if (command.out == NULL)
            { // out_str contains only whitespaces
                return false;
            }

            if (strtok(out_str, spaces) != NULL)
            { // multiple redirection destinations
                return false;
            }

            if (strsep(command_str, redirect_op) != NULL)
            { // multiple redirection operators
                return false;
            }
        }

        push_command(commands, command);
    }

    return true;
}

/* Execute parallel commands or single command.
Return true on success, false on failure.
Don't handle program errors (invoked programs handle their own errors).
*/
bool execute_commands(CommandArray *commands)
{
}

void handle(FILE *fp)
{
    CommandArray commands;
    init_command_array(&commands);

    char *line = NULL;
    size_t linelen = 0;

    while (getline(&line, &linelen, fp) != -1)
    {
        if (!parse_commands(line, &commands))
        {
            print_err();
            reset_command_array(&commands);
            continue;
        }

        if (commands.count == 0)
            continue;

        execute_commands(&commands);
        reset_command_array(&commands);
    }

    if (ferror(fp))
        panic();

    free(line);
}

void interactive_mode()
{
    handle(stdin);
    exit(EXIT_SUCCESS);
}

void batch_mode(char *filename)
{
    FILE *fp = Fopen(filename, "r");
    handle(fp);
    fclose(fp);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        interactive_mode();
    else if (argc == 2)
        batch_mode(argv[1]);
    else
        panic();

    return 0;
}