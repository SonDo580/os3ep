# 2: Try `free`

```bash
free -m

# Display memory in MiB
#
#         total   used    free    shared  buff/cache  available
# Mem:    7826    3042    822     402     3961        4051
# Swap:   4095    0       4095
```

- `Mem` is split into 3 main buckets:
  - `used`: actively used by programs
  - `buff/cache`: used by OS for performance optimization
  - `free`: untouched

```
used + (buff + cache) + free = 3042 + 3961 + 822 = 7825
discrepancy = 7826 - 7825 = 1
```

- discrepancy is caused by `shared` memory:
  - `tmpfs` files, shared libraries, ...
  - `shared` is counted in both `used` and `buff/cache` depending on how it's allocated.

- `available` memory :
  - Linux borrows empty space for disk caching (`buff/cache`)
  - If an application, needs memory, the kernel will evict data from the cache and give it to the application

```
available = free + evictable(buff + cache) <= 3961 + 822 = 4783
discrepancy = 4783 - 4051 = 732
```

# 4: Run `memory-user`

```bash
memory-user 500 # use 500 MiB

free -m
# Mem
#           total   used    free    shared  buff/cache  available
# Before:   7826    3198    660     422     3967        3904
# After:    7826    3725    133     422     3968        3377
```

- **Observations**:
  - `used` increases by 3725 - 3198 = 527 (> 500), which is the same as decreasing amount of `available` and `free`
  - `buff/cache` generally stays the same
  - `shared` stays the same

# 8: Try `pmap`

```bash
./memory-user.exe 500
# PID: 7175

pmap 7175
# 7175:   ./memory-user.exe 500
# 00006151ec319000      4K r---- memory-user.exe
# 00006151ec31a000      4K r-x-- memory-user.exe
# 00006151ec31b000      4K r---- memory-user.exe
# 00006151ec31c000      4K r---- memory-user.exe
# 00006151ec31d000      4K rw--- memory-user.exe
# 0000615222c72000    132K rw---   [ anon ]
# 0000703f565ff000 512004K rw---   [ anon ]
# 0000703f75a00000    160K r---- libc.so.6
# 0000703f75a28000   1620K r-x-- libc.so.6
# 0000703f75bbd000    352K r---- libc.so.6
# 0000703f75c15000      4K ----- libc.so.6
# 0000703f75c16000     16K r---- libc.so.6
# 0000703f75c1a000      8K rw--- libc.so.6
# 0000703f75c1c000     52K rw---   [ anon ]
# 0000703f75d9d000     12K rw---   [ anon ]
# 0000703f75db3000      8K rw---   [ anon ]
# 0000703f75db5000      8K r---- ld-linux-x86-64.so.2
# 0000703f75db7000    168K r-x-- ld-linux-x86-64.so.2
# 0000703f75de1000     44K r---- ld-linux-x86-64.so.2
# 0000703f75ded000      8K r---- ld-linux-x86-64.so.2
# 0000703f75def000      8K rw--- ld-linux-x86-64.so.2
# 00007ffc90268000    136K rw---   [ stack ]
# 00007ffc902f5000     16K r----   [ anon ]
# 00007ffc902f9000      8K r-x--   [ anon ]
# ffffffffff600000      4K --x--   [ anon ]
#  total           514788K
```

- `memory-user.exe` lines: the compiled program.
- [stack]: the execution stack.
- `libc.so.6` lines: compiled code of C standard library.
- `ld-linux-x86-64.so.2` lines: the dynamic linker.
- [anon]: anonymous memory, allocated by the OS, not backed by any file on disk.
  - including the `heap` (via `malloc`, `calloc`, `realloc`): see the line with `512004K`, which is memory dynamically allocated by `memory-user` program.
