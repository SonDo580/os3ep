#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "command.h"
#include "path.h"
#include "parse.h"
#include "exec.h"

void handle(FILE *fp)
{
    CommandArray commands;
    init_command_array(&commands);

    PathArray paths;
    init_path_array(&paths);
    add_default_paths(&paths);

    char *line = NULL;
    size_t linelen = 0;

    while (getline(&line, &linelen, fp) != -1)
    {
        if (!parse_commands(&line, &commands))
        {
            print_err();
            reset_command_array(&commands);
            continue;
        }

        if (commands.count == 0)
            continue;

        execute_commands(&commands, &paths);
        reset_command_array(&commands);
    }

    if (ferror(fp))
        panic();

    free(line);
    reset_path_array(&paths);
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