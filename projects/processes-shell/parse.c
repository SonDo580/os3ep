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
bool parse_commands(char **line, CommandArray *commands)
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
        char *args_str = strsep(&command_str, redirect_op);
        assert(args_str != NULL); // since command_str != ""
        if (*args_str == '\0')
        { // leading ">"
            return false;
        }

        char *arg;
        while ((arg = strtok(args_str, spaces)) != NULL)
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
        char *out_str = strsep(&command_str, redirect_op);
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

            if (strsep(&command_str, redirect_op) != NULL)
            { // multiple redirection operators
                return false;
            }
        }

        push_command(commands, command);
    }

    return true;
}