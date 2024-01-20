# Interpreter of Lama bytecode

Folders `lama-v1.20` with files from Lama repository (v 1.20):

* `runtime` - Lama runtime: GC, helping functions
* `byterun` - Lama bytecode disassembler. From there, the bytecode representation in C was used. 

 Command: ./lama-v1.20/byterun/byterun lama-v1.20/regression/test009.bc

* `regression` - test for interpreter correctness 
* `performance` - test on performance 

The Lama compiler contains a recursive interpreter and runs it with the `-i` option. The compiler generates bytecode with the `-b` option

## Stacks 
There are two stack: 

* **operand stack** stores arguments, local variables and return value. Use place handled by Lama gc between pointers [`__gc_stack_top`, `__gc_stack_bottom`).
* **call stack** stores return address, number of function arguments and locals. Only the return address and numbers are there, so we don't need to manage them with GC.

