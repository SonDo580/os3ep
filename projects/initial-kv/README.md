# kv

A simple persistent key-value store.

## Usage

```bash
./kv    # do nothing
./kv command1 <command2 ...>    # execute commands
```

## Supported commands

- put: `p,key,value` (key is an integer, value is a string without ',')
  - update entry value or insert entry.
- get: `g,key`
  - found -> print `<key>,<value>`.
  - not found -> print `<key> not found`.
- delete: `d,key`
  - found -> delete entry and print nothing.
  - not found -> print `<key> not found`.
- clear: `c`
  . removes all entries.
- all: `a`
  . print all entries in any order, with 1 entry per line.

## Persistence

- Use a single file 'database.txt' to store all entries in plain-text.
- 'kv' reads all the file into memory at startup, into a hash table.
- 'kv' updates the in-memory data structure when processing command.
- 'kv' writes all entries to the database file before exiting.

## Notes

- More sophisticated techniques are needed to handle very large database
  and tolerate crashes.
