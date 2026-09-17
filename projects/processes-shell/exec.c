#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "common.h"
#include "exec.h"

static void builtin_exit(ArgArray *args)
{
    if (args->count != 1)
        print_err();
    else
        exit(EXIT_SUCCESS);
}

static void builtin_cd(ArgArray *args)
{
    if (args->count != 2 || chdir(args->args[1]) == -1)
        print_err();
}

static void builtin_path(ArgArray *args, PathArray *paths)
{
    reset_path_array(paths);
    for (int i = 1; i < args->count; i++)
        push_path(paths, args->args[i]);
}

/* This function should not return. If it does, there was an error. */
static void execute_program(ArgArray *args, PathArray *paths, char *out)
{
    // Find executable
    char *executable_path = NULL;
    for (int i = 0; i < paths->count; i++)
    {
        char *dirpath = paths->paths[i];
        char *filename = args->args[0];
        assert(dirpath != NULL && filename != NULL);

        // filepath = "{dirpath}/{filename}"
        char filepath[strlen(dirpath) + 1 + strlen(filename) + 1];
        int i = 0;
        while (*dirpath != '\0')
            filepath[i++] = *dirpath;
        filepath[i++] = '/';
        while (*filename != '\0')
            filepath[i++] = *dirpath;
        filepath[i] = '\0';

        if (access(filepath, X_OK) == 0)
        {
            executable_path = filepath;
            break;
        }
    }
    if (executable_path == NULL)
        return;

    // Replace arg 0 and mark end of args
    replace_arg(args, 0, executable_path);
    push_arg(args, NULL);

    // Handle output redirection
    if (out != NULL)
    {
        int out_fd = open(out, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (out_fd == -1)
            return;

        if (dup2(out_fd, STDOUT_FILENO) == -1)
            return;
        if (dup2(out_fd, STDERR_FILENO) == -1)
            return;
        // dup2(fd, fd2) closes fd2 then makes fd2 open on the same file as fd.
        // -> stdout and stderr now shares the same file table entry and file offset.

        // Close original fd (file table entry is already referenced by stdout and stderr).
        if (close(out_fd) == -1)
            return;
    }

    // (this only returns if there was an error)
    execv(args->args[0], args->args);
}

/* Execute parallel commands or single command.
Don't handle program errors (invoked programs handle their own errors).
*/
void execute_commands(CommandArray *commands, PathArray *paths)
{
    assert(commands->count > 0 && commands->commands != NULL);

    int child_pids[commands->count];
    int child_count = 0;

    for (int i = 0; i < commands->count; i++)
    {
        Command *command = &commands->commands[i];

        ArgArray *args = &command->args;
        assert(args->count > 0 && args->args != NULL);
        char *program = args->args[0];

        if (strcmp(program, "exit") == 0)
            builtin_exit(args);
        else if (strcmp(program, "cd") == 0)
            builtin_cd(args);
        else if (strcmp(program, "path") == 0)
            builtin_path(args, paths);
        else
        {
            int rc = fork();
            if (rc == -1)
            { // fork failed
                print_err();
            }
            else if (rc == 0)
            { // child process goes down this path
                execute_program(args, paths, command->out);

                // If execute_program() returns, there was an error.
                print_err();
                exit(EXIT_FAILURE);
            }
            else
            { // parent process goes down this path
                child_pids[child_count] = rc;
                child_count++;
            }
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < child_count; i++)
        waitpid(child_pids[i], NULL, 0);
}
