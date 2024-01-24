#include <stddef.h>
#include <stdint.h>

#include "../lama-v1.20/runtime/gc.h"
#include "../lama-v1.20/runtime/runtime.h"
#include "../lama-v1.20/runtime/runtime_common.h"

// helper macros
#define ASSERT_TRUE(condition, msg, ...)                         \
    do                                                           \
        if (!(condition)) failure("\n" msg "\n", ##__VA_ARGS__); \
    while (0)

#define STRING get_string(bf, next_int())

//"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"
enum { PLUS, MINUS, MULT, DIV, MOD, LS, LE, GR, GE, EQ, NEQ, AND, OR };
#define BINOPS(def)                                                            \
    def(PLUS, +) def(MINUS, -) def(MULT, *) def(DIV, /) def(MOD, %) def(LS, <) \
        def(LE, <=) def(GR, >) def(GE, >=) def(EQ, ==) def(NEQ, !=)            \
            def(AND, &&) def(OR, ||)

// end of bytefile
unsigned char* eof;

/*THE HELPING CODE FROM BYTERUN*/
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

// variables needed for gc linkage
void* __start_custom_data = 0;
void* __stop_custom_data = 0;

// runtime imports
// dont have access to `runtime.c` methods, so defined it with `extern`
extern int Lread();
extern int Lwrite(int n);
extern void* Bstring(void* p);
extern void* Lstring(void* p);
extern int Llength(void* p);
extern void* Belem(void* p, int i);
extern void* Bsta(void* v, int i, void* x);
extern int LtagHash(char*);
extern int Btag(void* d, int t, int n);
extern void* Lstring(void* p);
extern int Bstring_patt(void* x, void* y);
extern int Barray_patt(void* d, int n);

/*
 * GLOBAL VARIABLES FOR INTERPRETER
 */

// constants
//  1 MB like in JVM by default + memory for globals
#define STACK_SIZE (1 << 20)
#define MEM_SIZE (STACK_SIZE * 2)
const int32_t EMPTY_BOX = BOX(0);

// area for global variables and stack
static int32_t gc_handled_memory[MEM_SIZE];
// area for call stack
static int32_t call_stack[STACK_SIZE];
// current instruction pointer
unsigned char* ip;
// address of current stack frame
int32_t* fp;
// call stack bottom pointer
int32_t* call_stack_bottom;
// call stack top pointer
int32_t* call_stack_top;
// start of gc handled memory and operands stack top
extern size_t __gc_stack_top;
// gc handled memory bottom
extern size_t __gc_stack_bottom;
// operands stack bottom and start of globals area
int32_t* globals;
// bytefile info
bytefile* bf;

int n_args = 0;
int n_locals = 0;
// needed to pop closure address from stack operands
bool is_closure = false;

/*
 * STACKS HANDLING
 */

// get operands stack top
static inline int32_t* sp() { return (int32_t*)__gc_stack_top; }

static inline void move_sp(int delta) {
    __gc_stack_top = (size_t)(sp() + delta);
}

static inline void push_op(int32_t value) {
    *sp() = value;
    move_sp(-1);
    ASSERT_TRUE(sp() != gc_handled_memory, "\nOperands stack overflow");
}

static inline void push_call(int32_t value) {
    *call_stack_top = value;
    call_stack_top--;
    ASSERT_TRUE(call_stack_top != call_stack, "\nCall stack overflow");
}

static inline int32_t pop_op(void) {
    ASSERT_TRUE(sp() != (int32_t*)__gc_stack_bottom - 1,
                "\nAccess to empty operands stack");
    move_sp(1);
    return *sp();
}

static inline int32_t peek_op(void) { return *(sp() + 1); }

static inline int32_t pop_call() {
    ASSERT_TRUE(call_stack_top != call_stack_bottom - 1,
                "\nAccess to empty call stack");
    call_stack_top++;
    return *call_stack_top;
}

/**
 * THE HELPING CODE FOR INTERPRETER
 */

void init(int32_t global_area_size) {
    // init GC heap, otherwise GC will fail (all heap pointers are 0)
    __init();
    __gc_stack_bottom = (size_t)gc_handled_memory + MEM_SIZE;
    globals = (int32_t*)__gc_stack_bottom - global_area_size;
    __gc_stack_top = (size_t)(globals - 1);

    call_stack_bottom = call_stack + STACK_SIZE;
    call_stack_top = call_stack_bottom - 1;

    // set boxed values in global area memory
    for (int i = 0; i < global_area_size; i++) {
        globals[i] = EMPTY_BOX;
    }
}

void update_ip(unsigned char* new_ip) {
    ASSERT_TRUE(
        new_ip >= bf->code_ptr && new_ip < eof,
        "IP points out of bytecode area! START_CODE: %d, EOF: %d, IP: %d",
        bf->code_ptr, eof, new_ip);
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

#define IS_MAIN (call_stack_top == call_stack_bottom - 1)

/**
 * METHODS FOR HANDLING BYTECODE
 */

inline static void call(void) {
    int32_t func_label = next_int();
    int32_t n_args = next_int();
    is_closure = false;

    push_call((int32_t)ip);  // return address
    update_ip(bf->code_ptr + func_label);
}

inline static void begin(int new_n_locs, int new_n_args) {
    // save frame pointer of callee function
    push_call((int32_t)fp);
    push_call(n_args);
    push_call(n_locals);
    push_call(is_closure);

    fp = sp();

    n_args = new_n_args, n_locals = new_n_locs;
    for (int i = 0; i < new_n_locs; i++) {
        push_op(EMPTY_BOX);
    }
}

inline static void tag(void) {
    // for pattern matching: check
    // that sexp has given tag and fields count
    char* tag = STRING;
    int32_t n_field = next_int();
    int32_t sexp = pop_op();
    int32_t tag_hash = LtagHash(tag);
    push_op(Btag((void*)sexp, tag_hash, BOX(n_field)));
}

inline static data* get_closure_content(int32_t* p) {
    data* closure_obj = TO_DATA(p);
    int t = TAG(closure_obj->data_header);
    ASSERT_TRUE(t == CLOSURE_TAG,
                "get_closure: pointer to not-closure object as argument");
    return closure_obj->contents;
}

inline static int32_t get_closure_addr(int32_t* p) {
    return ((int32_t*)get_closure_content(p))[0];
}

enum { G, L, A, C };
inline static int32_t* get_addr(int32_t place, int32_t idx) {
    ASSERT_TRUE(idx >= 0, "Index less than zero!!");
    switch (place) {
        case G:
            ASSERT_TRUE(globals + idx < (int32_t*)__gc_stack_bottom,
                        "Out of memory (global %d)", idx);
            return globals + idx;
        case L:
            ASSERT_TRUE(idx < n_locals, "Operands stack overflow!");
            return fp - idx;
        case A:
            ASSERT_TRUE(idx < n_args, "Arguments overflow!");
            return fp + n_args - idx;
        case C: {
            int32_t* closure_addr =
                get_closure_content((int32_t*)fp[n_args + 1]);
            return (closure_addr + idx + 1);
        }
        default:
            failure("Unknown place %d", place);
    }
}

static inline void ld(int32_t place_type, int idx) {
    int32_t* place = get_addr(place_type, idx);
    push_op(*place);
}

static inline void lda(int32_t place_type, int idx) {
    int32_t* place = get_addr(place_type, idx);
    push_op((int32_t)place);
}

static inline void st(int32_t place_type, int idx) {
    int32_t value = peek_op();
    int32_t* place = get_addr(place_type, idx);
    *place = value;
}

static inline void sta() {
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

// expired by function `Bclosure` from runtime.c
// create an object of closure ant put it on stack
inline static void closure(void) {
    int i, ai;
    data* r;

    void* closure_addr = (void*)next_int();
    // number of captured by closure variables
    int32_t n = next_int();

    r = (data*)alloc_closure(n + 1);

    push_extra_root((void**)&r);

    ((void**)r->contents)[0] = closure_addr;

    for (i = 0; i < n; i++) {
        unsigned char place_type = next_byte();
        int32_t idx = next_int();
        int32_t* place = get_addr(place_type, idx);
        ai = *place;
        ((int*)r->contents)[i + 1] = ai;
    }

    pop_extra_root((void**)&r);
    push_op((int32_t)r->contents);
}

// CALLC
inline static void call_closure(void) {
    int32_t n_args = next_int();
    // closure addr not in code -- it stored in closure object.
    // Stack store count of closure arguments and closure object
    // in 0 field in closure stored address, in other -- captured variables
    int32_t closure_label = get_closure_addr((int32_t*)sp()[n_args + 1]);
    is_closure = true;

    push_call((int32_t)ip);  // return address
    update_ip(bf->code_ptr + (int32_t)closure_label);
}

// CBEGIN
// Begin in closure if there has captured variables
// otherwise closure starts with `BEGIN`
inline static void begin_closure() {
    int new_n_args = next_int();
    int new_n_locs = next_int();
    begin(new_n_locs, new_n_args);
}

static inline void end() {
    int32_t return_val = pop_op();
    move_sp(n_args + n_locals);

    bool is_closure = pop_call();
    if (is_closure) {
        pop_op();
    }

    push_op(return_val);

    n_locals = pop_call();      // locs_n
    n_args = pop_call();        // args_n
    fp = (int32_t*)pop_call();  // fp

    if (call_stack_top != call_stack_bottom - 1) {
        ip = (unsigned char*)pop_call();  // ret addr
    }
}
void binop(int32_t operator_code) {
    int32_t b = pop_op(), a = pop_op();
    a = UNBOX(a), b = UNBOX(b);
    int32_t result = 0;
    switch (operator_code) {
#define IMPLEMENT_BINOP(n, op) \
    case n:                    \
        result = a op b;       \
        break;

        BINOPS(IMPLEMENT_BINOP)

#undef IMPLEMENT_BINOP
        default:
            failure("Unknown binop operand code: %d", operator_code);
    }
    result = BOX(result);
    push_op(result);
}

// inspired by `Barray` from runtime.c
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

// usually original method Bsexp
// called with args <fileds numbers + 1>
// so don't need create field `fields_count`
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

enum {
    str_literal,
    string_type,
    array_type,
    sexp_type,
    ref_type,
    val_type,
    closure_type
};

static inline bool check_tag(int32_t obj, int32_t tag) {
    if (UNBOXED(obj)) {
        return false;
    }
    int32_t actual_tag = TAG(TO_DATA(obj)->data_header);
    switch (tag) {
        case ref_type:
            return true;
        case str_literal: {
            int32_t other_str = pop_op();
            return UNBOX(Bstring_patt((void*)other_str, (void*)obj));
        }
        case string_type:
            return actual_tag == STRING_TAG;
        case array_type:
            return actual_tag == ARRAY_TAG;
        case sexp_type:
            return actual_tag == SEXP_TAG;
        case (closure_type):
            return actual_tag == CLOSURE_TAG;
        default:
            failure("There is no tag %d", tag);
    }
}

/**
Check that object from operands stack matches with pattern:
has the same type (tag) or equals as a string (in these case
other string stored on stack to)
*/
static inline void patt(int32_t patt_type) {
    bool result = false;
    int32_t obj = pop_op();

    if (patt_type == val_type) {
        result = UNBOXED(obj);
    } else {
        result = check_tag(obj, patt_type);
    }
    push_op(BOX(result));
}

// pattern matching with array
static inline void array(void) {
    int32_t array_size = BOX(next_int());
    int32_t actual_obj = pop_op();
    push_op(Barray_patt((void*)actual_obj, array_size));
}

// outer switch
enum { BINOP, H1_OPS, LD, LDA, ST, H5_OPS, PATT, H7_OPS };
// H1 OPS
enum { CONST, BSTRING, BSEXP, STI, STA, JMP, END, RET, DROP, DUP, SWAP, ELEM };
// H5 OPS
enum {
    CJMPZ,
    CJMPNZ,
    BEGIN,
    CBEGIN,
    BCLOSURE,
    CALLC,
    CALL,
    TAG,
    ARRAY_KEY,
    FAIL,
    LINE
};
// H7 OPS
enum { LREAD, LWRITE, LLENGTH, LSTRING, BARRAY };
static inline void interpret(FILE* f) {
    init(bf->global_area_size);
    ip = bf->code_ptr;
    do {
        char x = next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

        switch (h) {
            case 15:
                goto stop;

            case BINOP:
                binop(l - 1);
                break;
            case H1_OPS:
                switch (l) {
                    case CONST:
                        push_op(BOX(next_int()));
                        break;

                    case BSTRING: {
                        char* string_in_pool = STRING;
                        push_op((int32_t)Bstring(string_in_pool));
                        break;
                    }
                    case BSEXP:
                        call_bsexp();
                        break;

                    case STI:
                        failure("Untested operation STI");

                    case STA:
                        sta();
                        break;

                    case JMP: {  // JMP
                        int x = next_int();
                        update_ip(bf->code_ptr + x);
                        break;
                    }

                    case END:
                        end();
                        if (IS_MAIN) {
                            return;
                        }
                        break;
                    case RET:
                        failure("Untested operation RET");

                    case DROP:
                        pop_op();
                        break;

                    case DUP:
                        push_op(peek_op());
                        break;

                    case SWAP:
                        failure("Untested operation SWAP");

                    case ELEM: {  // ELEM
                        int32_t idx = pop_op();
                        int32_t array = pop_op();
                        push_op((int32_t)Belem((char*)array, idx));
                        break;
                    }
                    default:
                        failure("ERROR: invalid opcode %d-%d\n", h, l);
                }
                break;

            case LD:
                ld(l, next_int());
                break;
            case LDA:
                lda(l, next_int());
                break;
            case ST:
                st(l, next_int());
                break;

            case H5_OPS:
                switch (l) {
                    case CJMPZ: {
                        int x = next_int();
                        if (!UNBOX(pop_op())) {
                            update_ip(bf->code_ptr + x);
                        }
                        break;
                    }

                    case CJMPNZ: {  // CJMPnz
                        int x = next_int();
                        if (UNBOX(pop_op())) {
                            update_ip(bf->code_ptr + x);
                        }
                        break;
                    }

                    case BEGIN: {
                        int n_args = next_int();
                        int n_locs = next_int();
                        begin(n_locs, n_args);
                        break;
                    }

                    case CBEGIN:
                        begin_closure();
                        break;

                    case BCLOSURE:
                        closure();
                        break;

                    case CALLC:
                        call_closure();
                        break;
                    case CALL:
                        call();
                        break;
                    case TAG:
                        tag();
                        break;
                    case ARRAY_KEY:
                        array();
                        break;

                    case FAIL: {
                        int32_t line = next_int();
                        int32_t col = next_byte();
                        failure("\nFAIL at \t%d:%d", line, col);
                    }

                    /*information about source code line*/
                    case LINE:
                        next_int();
                        break;

                    default:
                        failure("ERROR: invalid opcode %d-%d\n", h, l);
                }
                break;

            case PATT:
                patt(l);
                break;

            case H7_OPS: {
                switch (l) {
                    case LREAD: {
                        // read make it BOX itself
                        int32_t value = Lread();
                        push_op(value);
                        break;
                    }

                    case LWRITE: {
                        int32_t value = pop_op();
                        value = Lwrite(value);
                        push_op(value);
                    } break;

                    case LLENGTH:
                        push_op(Llength((char*)pop_op()));
                        break;

                    case LSTRING:
                        push_op((int32_t)Lstring((char*)pop_op()));
                        break;

                    case BARRAY:
                        call_barray();
                        break;

                    default:
                        failure("ERROR: invalid opcode %d-%d\n", h, l);
                }
            } break;

            default:
                failure("ERROR: invalid opcode %d-%d\n", h, l);
        }
    } while (1);
stop:
    return;
}

int main(int argc, char* argv[]) {
    if(argc < 2){
        failure("Bad address!");
    }
    bf = read_file(argv[1]);
    interpret(stdout);
    return 0;
}
