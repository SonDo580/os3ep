# Operating Systems - 3 Easy Pieces

- Book: https://pages.cs.wisc.edu/~remzi/OSTEP/
- Homework: https://github.com/remzi-arpacidusseau/ostep-homework/
- Projects: https://github.com/remzi-arpacidusseau/ostep-projects/

# Useful commands

```bash
# hex-dump: Convert binary files to human-readable text format
xxd <binary>

# diff: Compare files (-u: use 'unified' format)
diff -u <file1> <file2>

# Example: Compare binary files
diff -u <(xxd tests/x.out) <(xxd tests-out/x.out)
```
