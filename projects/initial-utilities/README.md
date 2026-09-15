# wcat

print out each file in turn.

## Usage

```bash
wcat [files...]
```

# wgrep

For each file, print out the lines with user-specified search term.

## Usage

```bash
wgrep <keyword> [files...]
```

# wzip

Apply run-length encoding _(n same-type characters in a row = n + 1 character instance)_.

## Example

- "aaa" -> "3a".

## Usage

```bash
wzip [files...]
```

# wunzip

Decode output of `wzip`.

## Usage

```bash
wunzip [files...]
```