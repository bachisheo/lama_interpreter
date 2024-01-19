#include <stddef.h>
#include <stdint.h>

#include "../lama-v1.20/runtime/runtime.h"
#include "../lama-v1.20/runtime/runtime_common.h"

// variables needed for gc linkage
void* __start_custom_data;
void* __stop_custom_data;

// dont have access to runtime, define with `extern`
extern int Lread();
extern int Lwrite(int n);
#define EXPAND_VA_ARGS(...) , #__VA_ARGS__
#define ASSERT_TRUE(condition, msg, ...)              \
    do                                                \
        if (!(condition)) failure(msg, #__VA_ARGS__); \
    while (0)

// 1 MB like in JVM by default + memory for globals
#define STACK_SIZE (1 << 20)
#define MEM_SIZE (STACK_SIZE * 2)

// area for global variables and stack
static int32_t gc_handled_memory[MEM_SIZE];
// area for call stack
static int32_t call_stack[STACK_SIZE];

const char* pats[] = {"=str", "#string", "#array", "#sexp",
                      "#ref", "#val",    "#fun"};

/* The unpacked representation of bytecode file */
typedef struct {
    char* string_ptr;     /* A pointer to the beginning of the string table */
    int* public_ptr;      /* A pointer to the beginning of publics table    */
    char* code_ptr;       /* A pointer to the bytecode itself               */
    int* global_ptr;      /* A pointer to the global area                   */
    int stringtab_size;   /* The size (in bytes) of the string table        */
    int global_area_size; /* The size (in words) of global area             */
    int public_symbols_number; /* The number of public symbols */
    char buffer[0];
} bytefile;

/* Gets a string from a string table by an index */
char* get_string(bytefile* f, int pos) { return &f->string_ptr[pos]; }

/* Gets a name for a public symbol */
char* get_public_name(bytefile* f, int i) {
    return get_string(f, f->public_ptr[i * 2]);
}

/* Gets an offset for a publie symbol */
int get_public_offset(bytefile* f, int i) { return f->public_ptr[i * 2 + 1]; }

/* Reads a binary bytecode file by name and unpacks it */
bytefile* read_file(char* fname) {
    FILE* f = fopen(fname, "rb");
    long size;
    bytefile* file;

    if (f == 0) {
        failure("%s\n", strerror(errno));
    }

    if (fseek(f, 0, SEEK_END) == -1) {
        failure("%s\n", strerror(errno));
    }

    file = (bytefile*)malloc(sizeof(int) * 4 + (size = ftell(f)));

    if (file == 0) {
        failure("*** FAILURE: unable to allocate memory.\n");
    }

    // to the start of stream
    rewind(f);

    if (size != fread(&file->stringtab_size, 1, size, f)) {
        failure("%s\n", strerror(errno));
    }

    fclose(f);

    file->string_ptr =
        &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
    file->public_ptr = (int*)file->buffer;
    file->code_ptr = &file->string_ptr[file->stringtab_size];
    file->global_ptr = (int*)malloc(file->global_area_size * sizeof(int));

    return file;
}

#define addr int32_t
#define byte char

// current instruction pointer
addr ip;
// address of current stack frame
addr* fp;
// call stack
addr* call_stack_bottom;
// call stack pointer
addr* call_stack_top;
// base pointer ~ frame pointer
addr* bp;
// gc handled memory top
// and operands stack top
extern size_t __gc_stack_top;
// gc handled memory bottom
extern size_t __gc_stack_bottom;
// operands stack bottom
// and start of  globals area
addr* op_stack_bottom;
// globals area start
addr* globals;

int nArgs;
int nLocals;

// get operands stack top
static inline addr* sp() { return (addr*)__gc_stack_top; }

static inline void move_sp(int delta) {
    __gc_stack_top = (size_t)(sp() + delta);
}

void push_op(addr value) {
    *sp() = value;
    move_sp(-1);
    ASSERT_TRUE(sp() != gc_handled_memory, "\nOperands stack overflow");
}

void push_call(addr value) {
    *call_stack_top = value;
    call_stack_top--;
    ASSERT_TRUE(call_stack_top != call_stack, "\nCall stack overflow");
}

addr pop_op() {
    ASSERT_TRUE(sp() != (addr*)__gc_stack_bottom,
                "\nAccess to empty operands stack");
    move_sp(1);
    return *sp();
}

addr seek_op() { return *(sp() + 1); }

addr pop_call() {
    ASSERT_TRUE(call_stack_top != call_stack_bottom,
                "\nAccess to empty call stack");
    call_stack_top++;
    return *call_stack_top;
}

void begin(int nLocs, int nArgs) {
    // save frame pointer of callee function
    push_call((addr)bp);
    push_call(nArgs);
    push_call(nLocs);
    bp = sp();

    addr value = BOX(0);
    for (int i = 0; i < nLocs; i++) {
        push_op(value);
    }
}

void init(addr global_area_size) {
    __gc_stack_bottom = (size_t)gc_handled_memory + MEM_SIZE;
    op_stack_bottom = (addr*)__gc_stack_bottom - global_area_size;
    __gc_stack_top = (size_t)op_stack_bottom;
    globals = op_stack_bottom;
    globals++;

    call_stack_bottom = call_stack + STACK_SIZE;
    call_stack_top = call_stack_bottom;
}
enum { G, L, A, C };
void LD(addr place, int idx) {
    switch (place) {
        case G: {
            addr value = globals[idx];
            push_op(globals[idx]);
            break;
        }
        case L:
        case A:
        case C:
        default:
            failure("Unknown place for LD");
    }
}

void LDA(addr place, int idx) {
    switch (place) {
        case G:
        case L:
        case A:
        case C:
        default:
            failure("Unknown place for LDA");
    }
}

void ST(addr place, int idx) {
    switch (place) {
        case G: {
            addr value = seek_op();
            globals[idx] = value;
            break;
        }
        case L:
        case A:
        case C:
        default:
            failure("Unknown place for ST");
    }
}
//"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"
void binop(int32_t operator_code) {
    int32_t b = pop_op(), a = pop_op();
    a = UNBOX(a), b = UNBOX(b);
    int32_t result = 0;
    switch (operator_code) {
        case (0):
            result = a + b;
            break;
        case (1):
            result = a - b;
            break;
        case (2):
            result = a * b;
            break;
        case (3):
            result = a / b;
            break;
        case (4):
            result = a % b;
            break;
        case (5):
            result = a < b;
            break;
        case (6):
            result = a <= b;
            break;
        case (7):
            result = a > b;
            break;
        case (8):
            result = a >= b;
            break;
        case (9):
            result = a == b;
            break;
        case (10):
            result = a != b;
            break;
        case (11):
            result = a && b;
            break;
        case (12):
            result = a || b;
            break;
        default:
            failure("Unknown binop operand code: %d", operator_code);
    }
    result = BOX(result);
    push_op(result);
}
void interpret(FILE* f, bytefile* bf) {
#define INT (ip += sizeof(int), *(int*)(ip - sizeof(int)))
#define BYTE *ip++
#define STRING get_string(bf, INT)
#define FAIL failure("ERROR: invalid opcode %d-%d\n", h, l)

    init(bf->global_area_size);

    char* ip = bf->code_ptr;

    do {
        // read next byte
        char x = BYTE, h = (x & 0xF0) >> 4, l = x & 0x0F;

        switch (h) {
            case 15:
                goto stop;

            /* BINOP */
            case 0:
                binop(l - 1);
                break;

            case 1:
                switch (l) {
                    case 0:
                        push_op(BOX(INT));
                        break;

                    case 1:
                        failure("\nDont implement for STRING\t%s", STRING);
                        break;

                    case 2:
                        failure("\nDont implement for SEXP\t%s ", STRING);
                        failure("\nDont implement for %d", INT);
                        break;

                    case 3:
                        failure("\nDont implement for STI");
                        break;

                    case 4:
                        failure("\nDont implement for STA");
                        break;

                    case 5:
                        failure("\nDont implement for JMP\t0x%.8x", INT);
                        break;

                    case 6:
                        // failure("\nDont implement for END");
                        break;

                    case 7:
                        failure("\nDont implement for RET");
                        break;

                    case 8:
                        // DROP
                        pop_op();
                        break;

                    case 9:
                        failure("\nDont implement for DUP");
                        break;

                    case 10:
                        failure("\nDont implement for SWAP");
                        break;

                    case 11:
                        failure("\nDont implement for ELEM");
                        break;

                    default:
                        FAIL;
                }
                break;

            case 2:
                LD(l, INT);
                break;
            case 3:
                LDA(l, INT);
                break;
            case 4: {
                ST(l, INT);
                break;
            }

            case 5:
                switch (l) {
                    case 0:
                        failure("\nDont implement for CJMPz\t0x%.8x", INT);
                        break;

                    case 1:
                        failure("\nDont implement for CJMPnz\t0x%.8x", INT);
                        break;

                    case 2:
                        begin(INT, INT);
                        break;

                    case 3:
                        failure("\nDont implement for CBEGIN\t%d ", INT);
                        failure("\nDont implement for %d", INT);
                        break;

                    case 4:
                        failure("\nDont implement for CLOSURE\t0x%.8x", INT);
                        {
                            int n = INT;
                            for (int i = 0; i < n; i++) {
                                switch (BYTE) {
                                    case 0:
                                        failure("\nDont implement for G(%d)",
                                                INT);
                                        break;
                                    case 1:
                                        failure("\nDont implement for L(%d)",
                                                INT);
                                        break;
                                    case 2:
                                        failure("\nDont implement for A(%d)",
                                                INT);
                                        break;
                                    case 3:
                                        failure("\nDont implement for C(%d)",
                                                INT);
                                        break;
                                    default:
                                        FAIL;
                                }
                            }
                        };
                        break;

                    case 5:
                        failure("\nDont implement for CALLC\t%d", INT);
                        break;

                    case 6:
                        failure("\nDont implement for CALL\t0x%.8x ", INT);
                        failure("\nDont implement for %d", INT);
                        break;

                    case 7:
                        failure("\nDont implement for TAG\t%s ", STRING);
                        failure("\nDont implement for %d", INT);
                        break;

                    case 8:
                        failure("\nDont implement for ARRAY\t%d", INT);
                        break;

                    case 9:
                        failure("\nDont implement for FAIL\t%d", INT);
                        failure("\nDont implement for %d", INT);
                        break;

                    case 10:
                        // helper information about source code line
                        INT;
                        break;

                    default:
                        FAIL;
                }
                break;

            case 6:
                failure("\nDont implement for PATT\t%s", pats[l]);
                break;

            case 7: {
                switch (l) {
                    case 0: {
                         addr value = Lread();
                        push_op(value);
                        break;
                    }

                    case 1: {
                        addr value = pop_op();
                        value = Lwrite(value);
                        push_op(value);
                    } break;

                    case 2:
                        failure("\nDont implement for CALL\tLlength");
                        break;

                    case 3:
                        failure("\nDont implement for CALL\tLstring");
                        break;

                    case 4:
                        failure("\nDont implement for CALL\tBarray\t%d", INT);
                        break;

                    default:
                        FAIL;
                }
            } break;

            default:
                FAIL;
        }
    } while (1);
stop:
    return;
}

int main(int argc, char* argv[]) {
    bytefile* f = read_file(argv[1]);
    interpret(stdout, f);
    return 0;
}
