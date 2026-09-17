#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "parse.h"

const char *ampersand_op = "&";
const char *redirect_op = ">";
const char *spaces = " \t\n";

/* Parse commands from input line.
Return true on success, false on failure.

- Example: `ls -la . > out.txt & wc wish.c`
- Parsed:
```
[
  {args=["ls", "-la", ".", NULL], out="out.txt"},
  {args=["wc, "wish.c", NULL], out=NULL},
]
```
*/
bool parse_commands(char *line, CommandArray *commands)
{
    char *input = line;
    char *command_str;
    bool encountered_only_spaces_command = false;
    bool encountered_empty_command = false;

    while ((command_str = strsep(&input, ampersand_op)) != NULL)
    {
        if (encountered_only_spaces_command)
            return false; // parallel commands must not have any only-spaces (<-> empty) command
        if (encountered_empty_command)
            return false; // parallel commands must not have any empty command

        if (*command_str == '\0')
        { // allow skipping single empty command, fail if detect next command
            encountered_empty_command = true;
            continue;
        }

        Command command;
        init_command(&command);

        // Parse arguments
        // args_str: full command_str or before ">"
        char *args_str = strsep(&command_str, redirect_op);
        assert(args_str != NULL); // since command_str != ""
        if (*args_str == '\0')
            return false; // leading ">"

        char *arg;
        while ((arg = strsep(&args_str, spaces)) != NULL)
        {
            if (*arg == '\0')
                continue; // skip empty tokens
            push_arg(&command.args, arg);
        }

        if (command.args.count == 0)
        { // args_str contains only whitespaces
            if (command_str != NULL)
                return false; // args_str is followed by ">" (doesn't match full command_str)
            else if (commands->count > 0)
                return false; // parallel commands must not have any only-spaces (<-> empty) command
            else
            { // allow skipping single only-spaces (<-> empty) comand, fail if detect next command
                encountered_only_spaces_command = true;
                continue;
            }
        }

        // Parse redirect output
        // out_str: empty or after ">"
        char *out_str = strsep(&command_str, redirect_op);
        if (out_str != NULL)
        {
            if (*out_str == '\0')
                return false; // trailing ">"

            char *out;
            while ((out = strsep(&out_str, spaces)) != NULL)
            {
                if (*out == '\0')
                    continue; // skip empty tokens
                if (command.out != NULL)
                    return false; // multiple redirection destinations
                command.out = out;
            }

            if (command.out == NULL)
                return false; // out_str contains only whitespaces

            if (strsep(&command_str, redirect_op) != NULL)
                return false; // multiple redirection operators
        }

        push_command(commands, command);
    }

    return true;
}