# wish

A command line interpreter _(i.e. a shell)_.

## Usage

```bash
# Build
make wish

# Interactive mode: show prompt, user types in the commands
./wish

# Batch mode: read and execute commands from a file
./wish <file>
```

## Paths

- The set of directories to search for executables.
- Default path contains 1 directory: `/bin`. Can overwrite with the builtin `path` command.
- **Don't support** the following features:
  - specify an **absolute path** (example: `/bin/ls`).
  - specify a **relative path** (example: `./main`).

## Builtin commands

```bash
# Exit the shell
exit

# Change directory
cd <path>

# Specify search path (overwrite)
path <space-separated paths>
# Example
path /bin /usr/bin
```

## Redirection

- Reroute standard output and standard error of the program to the specified file. Overwrite if output file already exists.
- Example: `ls -la /tmp > output`
- **Don't support** redirection for builtin commands (example: `path /bin > file`).

## Parallel commands

- Run commands (with arguments) in parallel before waiting for any them to complete.
- Example: `cmd1 & cmd2 args1 args2 & cmd3 args1`.

## Syntax notes

- Allow variable amount of whitespaces (' ', '\t') before and after commands, arguments, operators.
- Operators ('>', '&') do not require whitespaces.
- **Don't allow** empty component before/after '>'.
- **Don't allow** empty component in parallel commands _(leading/trailing/consecutive '&')_.

## Debugging tips (`GDB`)

```bash
# Keep both parent and child processes suspended and attached
set detach-on-fork off

# (Optional) Switch focus to child process after the fork
set follow-fork-mode child

# List running processes
info inferiors

# Switch between parent and child
inferior <num>
```

## My modification to supplied `tests`

- **3**:
  - Don't set `path` -> Search in `/bin`.
  - The error shows absolute path, not just program name.

```bash
# === 3.pre ===
# before
ls: cannot access '/no/such/file': No such file or directory
# after
/bin/ls: cannot access '/no/such/file': No such file or directory
```

- **16**, **17**:
  - The tests accept leading/trailing/consecutive '&'.
  - I treat those cases as errors (standard shell behavior).

```bash
# === 16.err, 17.err ===
# before: (empty)
# after
An error has occurred

# === 17.out ===
# before: ...
# after: (empty)

```
