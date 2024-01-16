# Interpreter of Lama bytecode

Folders from Lama repository (v 1.20):

* `runtime` - Lama runtime: GC, helping functions
* `byterun` - Lama bytecode disassembler. From there, the bytecode representation in C was used 
* `regression` - test for interpreter correctness 
* `performance` - test on performance 

The Lama compiler contains a recursive interpreter and runs it with the `-i` option. The compiler generates bytecode with the `-b` option