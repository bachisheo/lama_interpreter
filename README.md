# Learning repository for working with LAMA programming language



## Project structure

Folders `lama-v1.20` with files from Lama repository (v 1.20):

* `runtime` - Lama runtime: GC, helping functions
* `byterun` - Lama bytecode disassembler. From there, the bytecode representation in C was used. 
* `interpreter` - Implementation of iterative interpreter of Lama bytecode.
* `frequency` - Lama bytecode frequency analysis

## Useful Lama compiler features

The compiler generates bytecode of `.lama` files with the `-b` option:

```
lamac -b <file_name>.lama
```

Pretty print for bytecode:

```
./lama-v1.20/byterun/byterun <file_name>.bc
```