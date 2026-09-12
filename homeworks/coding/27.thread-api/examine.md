# 1

```bash
valgrind --tool=helgrind ./main-race.exe
```

- Report 2 errors (possible data race) at line 11 and 19 (update `balance`).

# 2: Modify `main-race.c`

- remove 1 update -> 0 errors.
- add lock around 1 update -> still 2 errors (all concurrent accesses to a shared variable must be protected by the **same** lock).
- add lock around both updates -> 0 errors.

# 3: Problem of `main-deadlock.c`

- 2 threads acquire locks m1, m2 in opposite order.
- Example flow:
  - thread 1 acquires m1 -> success.
  - thread 2 acquires m2 -> success.
  - thread 1 tries to acquire m2 -> block (held by thread 2).
  - thread 2 tries to acquire m1 -> block (held by thread 1).

# 4

```bash
valgrind --tool=helgrind ./main-deadlock.exe
```

- Report lock-order violation (potential deadlock).

# 5

```bash
valgrind --tool=helgrind ./main-deadlock-global.exe
```

- Still report lock-order violation, although we have protect the whole section with an additional mutex.
- Conclusion: `helgrind` performs static analysis instead of runtime check.

# 6: Why `main-signal.c` is inefficient

- Because it wastes CPU cycles (busy waiting) instead of putting the thread to sleep.

# 7

```bash
valgrind --tool=helgrind ./main-signal.exe
```

- Report possible data race (reading `done` at line 20 conflicts with writing `done` at line 12).

# 8: `main-signal-cv.c`

- This prevents race condition and is more efficient than `main-signal.c`.

# 9

```bash
valgrind --tool=helgrind ./main-signal-cv.exe
```

- Report no errors.
