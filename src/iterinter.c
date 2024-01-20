#include <stddef.h>
#include <stdint.h>

#include "../lama-v1.20/runtime/gc.h"
#include "../lama-v1.20/runtime/runtime.h"
#include "../lama-v1.20/runtime/runtime_common.h"

// variables needed for gc linkage
void* __start_custom_data = 0;
void* __stop_custom_data = 0;

// dont have access to runtime, define with `extern`
extern int Lread();
extern int Lwrite(int n);
extern void* Bstring(void* p);
extern int Llength(void* p);
extern void* Belem(void* p, int i);
extern void* Bsta(void* v, int i, void* x);
extern int LtagHash(char*);
extern int Btag(void* d, int t, int n);

#define ASSERT_TRUE(condition, msg, ...)                    \
    do                                                      \
        if (!(condition)) failure("\n" msg, ##__VA_ARGS__); \
    while (0)

#define STRING get_string(bf, next_int())

// 1 MB like in JVM by default + memory for globals
#define STACK_SIZE (1 << 20)
#define MEM_SIZE (STACK_SIZE * 2)

// area for global variables and stack
static int32_t gc_handled_memory[MEM_SIZE];
// area for call stack
static int32_t call_stack[STACK_SIZE];

// end of bytefile
unsigned char* eof;

const int32_t EMPTY_BOX = BOX(0);
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
char* get_string(bytefile* f, int pos) {
    ASSERT_TRUE(pos >= 0 && pos < f->stringtab_size,
                "Index out of string pool!");
    return &f->string_ptr[pos];
}

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

    int file_size = sizeof(int) * 4 + (size = ftell(f));
    file = (bytefile*)malloc(file_size);
    eof = (unsigned char*)file + file_size;

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
unsigned char* ip;
// address of current stack frame
addr* fp;
// call stack
addr* call_stack_bottom;
// call stack pointer
addr* call_stack_top;
// gc handled memory top
// and operands stack top
extern size_t __gc_stack_top;
// gc handled memory bottom
extern size_t __gc_stack_bottom;
// operands stack bottom
// and start of  globals area
addr* globals;
// bytefile info
bytefile* bf;

int n_args;
int n_locals;

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

addr pop_op(void) {
    ASSERT_TRUE(sp() != (addr*)__gc_stack_bottom - 1,
                "\nAccess to empty operands stack");
    move_sp(1);
    return *sp();
}

addr peek_op() { return *(sp() + 1); }

addr pop_call() {
    ASSERT_TRUE(call_stack_top != call_stack_bottom - 1,
                "\nAccess to empty call stack");
    call_stack_top++;
    return *call_stack_top;
}

void init(addr global_area_size) {
    // init GC heap, otherwise GC will fail (all heap pointers are 0)
    __init();
    __gc_stack_bottom = (size_t)gc_handled_memory + MEM_SIZE;
    globals = (addr*)__gc_stack_bottom - global_area_size;
    __gc_stack_top = (size_t)(globals - 1);

    call_stack_bottom = call_stack + STACK_SIZE;
    call_stack_top = call_stack_bottom - 1;

    // set boxed values in global area memory
    for (int i = 0; i < global_area_size; i++) {
        globals[i] = EMPTY_BOX;
    }
}

void update_ip(unsigned char* new_ip) {
    ASSERT_TRUE(new_ip >= bf->code_ptr && new_ip < eof,
                "IP points out of bytecode area!");
    ip = new_ip;
}

inline static unsigned char next_byte() {
    ASSERT_TRUE(ip + 1 < eof, "IP points out of bytecode area!");
    return *ip++;
}

inline static int32_t next_int() {
    ASSERT_TRUE(ip + sizeof(int) < eof, "IP points out of bytecode area!");
    return (ip += sizeof(int), *(int*)(ip - sizeof(int)));
}

inline static void call() {
    int32_t func_label = next_int();
    int32_t n_args = next_int();

    push_call((addr)ip);  // return address
    update_ip(bf->code_ptr + func_label);
}

void begin(int new_n_locs, int new_n_args) {
    // save frame pointer of callee function
    push_call((addr)fp);
    push_call(n_args);
    push_call(n_locals);
    fp = sp();

    n_args = new_n_args, n_locals = new_n_locs;
    addr value = BOX(0);
    for (int i = 0; i < new_n_locs; i++) {
        push_op(value);
    }
}

#define IS_MAIN (call_stack_top == call_stack_bottom - 1)
static inline void end() {
    addr return_val = pop_op();
    move_sp(n_args + n_locals);
    push_op(return_val);

    n_locals = pop_call();   // locs_n
    n_args = pop_call();     // args_n
    fp = (addr*)pop_call();  // fp

    if (call_stack_top != call_stack_bottom - 1) {
        ip = (unsigned char*)pop_call();  // ret addr
    }
}

void tag(void) {
    // for pattern matching: check
    // that sexp has given tag and fields count
    char* tag = STRING;
    int32_t n_field = next_int();
    int32_t sexp = pop_op();
    int32_t tag_hash = LtagHash(tag);
    push_op(Btag((void*)sexp, tag_hash, BOX(n_field)));
}

enum { G, L, A, C };
addr* get_addr(addr place, addr idx) {
    ASSERT_TRUE(idx >= 0, "Index less than zero!!");
    switch (place) {
        case G:
            ASSERT_TRUE(globals + idx < (addr*)__gc_stack_bottom,
                        "Out of memory (global %d)", idx);
            return globals + idx;
        case L:
            ASSERT_TRUE(idx < n_locals, "Operands stack overflow!");
            return fp - idx;
        case A:
            ASSERT_TRUE(idx < n_args, "Arguments overflow!");
            return fp + idx + 1;
        case C:
        default:
            failure("Unknown place %d", place);
    }
}

void LD(int32_t place_type, int idx) {
    addr* place = get_addr(place_type, idx);
    push_op(*place);
}

void LDA(int32_t place_type, int idx) {
    addr* place = get_addr(place_type, idx);
    push_op((int32_t)place);
}

void ST(addr place_type, int idx) {
    addr value = peek_op();
    addr* place = get_addr(place_type, idx);
    *place = value;
}

void STA() {
    int32_t value = pop_op();
    int32_t dest = pop_op();
    if (UNBOXED(dest)) {
        int32_t array = pop_op();
        Bsta((void*)value, dest, (void*)array);
    } else {
        *(int32_t*)dest = value;
    }
    push_op(value);
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

// copy logic from `Barray` in runtime.c
static inline void call_barray(void) {
    int i, ai;
    data* r;
    int n = next_int();

    r = (data*)alloc_array(n);

    for (i = n - 1; i >= 0; i--) {
        ai = pop_op();
        ((int*)r->contents)[i] = ai;
    }
    push_op((int32_t)r->contents);
}

//usually original method Bsexp 
//called with args <fileds numbers + 1>
//so don't need create field `fields_count`
static inline void call_bsexp(void) {
    int i;
    int ai;
    size_t* p;
    data* r;
    char* tag = STRING;
    int n = next_int();
    r = (data*)alloc_sexp(n);
    ((sexp*)r)->tag = 0;

    for (i = n; i >= 1; i--) {
        ai = pop_op();
        ((int*)r->contents)[i] = ai;
    }

    ((sexp*)r)->tag = UNBOX(LtagHash(tag));

    push_op((int32_t)r->contents);
}

void interpret(FILE* f) {
#define FAIL failure("ERROR: invalid opcode %d-%d\n", h, l)

    init(bf->global_area_size);

    ip = bf->code_ptr;

    do {
        // read next byte
        char x = next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

        switch (h) {
            case 15:
                goto stop;

            /* BINOP */
            case 0:
                binop(l - 1);
                break;

            case 1:
                switch (l) {
                    case 0:  // CONST
                        push_op(BOX(next_int()));
                        break;

                    case 1: {
                        char* string_in_pool = STRING;
                        push_op((int32_t)Bstring(string_in_pool));
                        break;
                    }

                    case 2:
                        call_bsexp();
                        break;

                    case 3:
                        failure("\nDont implement for STI");
                        break;

                    case 4:
                        STA();
                        break;

                    case 5:
                        // JMP
                        {
                            int x = next_int();
                            update_ip(bf->code_ptr + x);
                            break;
                        }

                    case 6:
                        end();
                        if (IS_MAIN) {
                            return;
                        }
                        break;
                    case 7:
                        failure("\nDont implement for RET");
                        break;

                    case 8:
                        // DROP
                        pop_op();
                        break;

                    case 9:
                        // DUP
                        push_op(peek_op());
                        break;

                    case 10:
                        failure("\nDont implement for SWAP");
                        break;

                    case 11: {
                        int32_t idx = pop_op();
                        int32_t array = pop_op();
                        push_op((int32_t)Belem((char*)array, idx));
                        break;
                    }
                    default:
                        FAIL;
                }
                break;

            case 2:
                LD(l, next_int());
                break;
            case 3:
                LDA(l, next_int());
                break;
            case 4: {
                ST(l, next_int());
                break;
            }

            case 5:
                switch (l) {
                    case 0: {  // CJMPz
                        int x = next_int();
                        if (!UNBOX(pop_op())) {
                            update_ip(bf->code_ptr + x);
                        }
                        break;
                    }

                    case 1: {  // CJMPnz
                        int x = next_int();
                        if (UNBOX(pop_op())) {
                            update_ip(bf->code_ptr + x);
                        }
                        break;
                    }

                    case 2: {
                        int n_args = next_int();
                        int n_locs = next_int();
                        begin(n_locs, n_args);
                        break;
                    }

                    case 3:
                        failure("\nDont implement for CBEGIN\t%d ", next_int());
                        failure("\nDont implement for %d", next_int());
                        break;

                    case 4:
                        failure("\nDont implement for CLOSURE\t0x%.8x",
                                next_int());
                        {
                            int n = next_int();
                            for (int i = 0; i < n; i++) {
                                switch (next_byte()) {
                                    case 0:
                                        failure("\nDont implement for G(%d)",
                                                next_int());
                                        break;
                                    case 1:
                                        failure("\nDont implement for L(%d)",
                                                next_int());
                                        break;
                                    case 2:
                                        failure("\nDont implement for A(%d)",
                                                next_int());
                                        break;
                                    case 3:
                                        failure("\nDont implement for C(%d)",
                                                next_int());
                                        break;
                                    default:
                                        FAIL;
                                }
                            }
                        };
                        break;
                    case 5:
                        failure("\nDont implement for CALLC\t%d", next_int());
                        break;
                    case 6:
                        call();
                        break;
                    case 7:
                        tag();
                        break;

                    case 8:
                        failure("\nDont implement for ARRAY\t%d", next_int());
                        break;

                    case 9:
                        failure("\nDont implement for FAIL\t%d", next_int());
                        failure("\nDont implement for %d", next_int());
                        break;

                    case 10:
                        // LINE -- helper information about source code line
                        next_int();
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
                        // read make it BOX
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
                        push_op(Llength((char*)pop_op()));
                        break;

                    case 3:
                        failure("\nDont implement for CALL\tLstring");
                        break;

                    case 4:
                        call_barray();
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
    bf = read_file(argv[1]);
    interpret(stdout);
    return 0;
}
