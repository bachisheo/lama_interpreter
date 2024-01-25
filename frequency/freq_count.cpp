#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// end of bytefile
uint8_t* eof;

/*THE HELPING CODE FROM BYTERUN*/
/* The unpacked representation of bytecode file */
struct bytefile {
    const char* string_ptr; /* A pointer to the beginning of the string table */
    const int* public_ptr;  /* A pointer to the beginning of publics table    */
    const uint8_t* code_ptr; /* A pointer to the bytecode itself */
    const int* global_ptr; /* A pointer to the global area                   */
    unsigned int stringtab_size; /* The size (in bytes) of the string table   */
    unsigned int global_area_size;      /* The size (in words) of global area */
    unsigned int public_symbols_number; /* The number of public symbols */
    char buffer[0];
};

void failure(const char* patt, const char* arg) {
    printf("\nERROR!\n");
    printf(patt, arg);
    exit(1);
}
void failure(const char* patt, int l, int r) {
    printf("\nERROR!\n");
    printf(patt, l, r);
    exit(1);
}
void failure(const char* msg) {
    std::cout << "\nERROR!\n" << msg;
    exit(1);
}

#define ASSERT_TRUE(condition, msg, ...)                         \
    do                                                           \
        if (!(condition)) failure("\n" msg "\n", ##__VA_ARGS__); \
    while (0)

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
    eof = (uint8_t*)file + file_size;

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
    file->code_ptr = (uint8_t*)&file->string_ptr[file->stringtab_size];

    return file;
}

struct byte_reader {
    byte_reader(uint8_t* eof, bytefile* bf)
        : eof(eof), is_print(false), bf(bf) {}

    byte_reader(uint8_t* eof, bytefile* bf, char* buf, size_t buf_size)
        : eof(eof), is_print(true), bf(bf), buf(buf), buf_size(buf_size) {}

    const uint8_t* read_instruction(const uint8_t* ip) {
        written = 0;
        this->ip = ip;
        return read_instruction();
    }

    char* get_str() { return buf; }

   private:
    bytefile* bf;
    uint8_t* eof;
    size_t buf_size = 0;
    char* buf = nullptr;
    bool is_print;
    const uint8_t* ip;
    size_t written = 0;

    void cond_print(const char* fmt, ...) {
        if (!is_print) return;
        va_list args;
        va_start(args, fmt);
        written += vsnprintf(buf + written, buf_size - written, fmt, args);
    }

    /* Gets a string from a string table by an index */
    const char* get_string(int pos) {
        ASSERT_TRUE(unsigned(pos) < unsigned(bf->stringtab_size),
                    "Index out of string pool!");
        return &bf->string_ptr[pos];
    }

    int32_t next_int() {
        ASSERT_TRUE(ip + sizeof(int) < eof, "IP points out of bytecode area!");
        return (ip += sizeof(int), *(int*)(ip - sizeof(int)));
    }

    unsigned char next_byte() {
        ASSERT_TRUE(ip < eof, "IP points out of bytecode area!");
        return *ip++;
    }

    /* Disassembles the bytecode */
    const uint8_t* read_instruction() {
#define INT next_int()
#define BYTE next_byte()
#define STRING get_string(INT)
#define FAIL failure("ERROR: invalid opcode %d-%d\n", h, l)

        const char* ops[] = {"+", "-",  "*",  "/",  "%",  "<", "<=",
                             ">", ">=", "==", "!=", "&&", "!!"};
        const char* pats[] = {"=str", "#string", "#array", "#sexp",
                              "#ref", "#val",    "#fun"};
        const char* lds[] = {"LD", "LDA", "ST"};
        uint8_t x = BYTE, h = (x & 0xF0) >> 4, l = x & 0x0F;

        switch (h) {
            case 15:
                cond_print("STOP");
                break;

            /* BINOP */
            case 0:
                cond_print("BINOP\t%s", ops[l - 1]);
                break;

            case 1:
                switch (l) {
                    case 0:
                        cond_print("CONST\t%d", INT);
                        break;

                    case 1:
                        cond_print("STRING\t%s", STRING);
                        break;

                    case 2:
                        cond_print("SEXP\t%s ", STRING);
                        cond_print("%d", INT);
                        break;

                    case 3:
                        cond_print("STI");
                        break;

                    case 4:
                        cond_print("STA");
                        break;

                    case 5:
                        cond_print("JMP\t0x%.8x", INT);
                        break;

                    case 6:
                        cond_print("END");
                        break;

                    case 7:
                        cond_print("RET");
                        break;

                    case 8:
                        cond_print("DROP");
                        break;

                    case 9:
                        cond_print("DUP");
                        break;

                    case 10:
                        cond_print("SWAP");
                        break;

                    case 11:
                        cond_print("ELEM");
                        break;

                    default:
                        FAIL;
                }
                break;

            case 2:
            case 3:
            case 4:
                cond_print("%s\t", lds[h - 2]);
                switch (l) {
                    case 0:
                        cond_print("G(%d)", INT);
                        break;
                    case 1:
                        cond_print("L(%d)", INT);
                        break;
                    case 2:
                        cond_print("A(%d)", INT);
                        break;
                    case 3:
                        cond_print("C(%d)", INT);
                        break;
                    default:
                        FAIL;
                }
                break;

            case 5:
                switch (l) {
                    case 0:
                        cond_print("CJMPz\t0x%.8x", INT);
                        break;

                    case 1:
                        cond_print("CJMPnz\t0x%.8x", INT);
                        break;

                    case 2:
                        cond_print("BEGIN\t%d ", INT);
                        cond_print("%d", INT);
                        break;

                    case 3:
                        cond_print("CBEGIN\t%d ", INT);
                        cond_print("%d", INT);
                        break;

                    case 4:
                        cond_print("CLOSURE\t0x%.8x", INT);
                        {
                            int n = INT;
                            for (int i = 0; i < n; i++) {
                                switch (BYTE) {
                                    case 0:
                                        cond_print("G(%d)", INT);
                                        break;
                                    case 1:
                                        cond_print("L(%d)", INT);
                                        break;
                                    case 2:
                                        cond_print("A(%d)", INT);
                                        break;
                                    case 3:
                                        cond_print("C(%d)", INT);
                                        break;
                                    default:
                                        FAIL;
                                }
                            }
                        };
                        break;

                    case 5:
                        cond_print("CALLC\t%d", INT);
                        break;

                    case 6:
                        cond_print("CALL\t0x%.8x ", INT);
                        cond_print("%d", INT);
                        break;

                    case 7:
                        cond_print("TAG\t%s ", STRING);
                        cond_print("%d", INT);
                        break;

                    case 8:
                        cond_print("ARRAY\t%d", INT);
                        break;

                    case 9:
                        cond_print("FAIL\t%d", INT);
                        cond_print("%d", INT);
                        break;

                    case 10:
                        cond_print("LINE\t%d", INT);
                        break;

                    default:
                        FAIL;
                }
                break;

            case 6:
                cond_print("PATT\t%s", pats[l]);
                break;

            case 7: {
                switch (l) {
                    case 0:
                        cond_print("CALL\tLread");
                        break;

                    case 1:
                        cond_print("CALL\tLwrite");
                        break;

                    case 2:
                        cond_print("CALL\tLlength");
                        break;

                    case 3:
                        cond_print("CALL\tLstring");
                        break;

                    case 4:
                        cond_print("CALL\tBarray\t%d", INT);
                        break;

                    default:
                        FAIL;
                }
            } break;

            default:
                FAIL;
        }

        return ip;
    }
};

struct instruction {
    instruction(const uint8_t* start, const uint8_t* end)
        : begin(start), end(end) {}

    bool operator<(instruction other) const {
        auto res = memcmp(begin, other.begin, end - begin);
        return res < 0;
    }

    const uint8_t* get_begin() { return begin; }

   private:
    const uint8_t* begin;
    const uint8_t* end;
};

struct freq_counter {
    freq_counter(bytefile* bf) : bf(bf) {}
    void print_frequency() {
        calculate_codes();
        std::vector<std::pair<instruction, int>> sorted_codes(codes.begin(),
                                                              codes.end());
        std::sort(sorted_codes.begin(), sorted_codes.end(),
                  [](auto& a, auto& b) { return a.second > b.second; });
        const size_t buf_size = 1024;
        char buf[buf_size];
        byte_reader br(eof, bf, buf, buf_size);
        for (auto& it : sorted_codes) {
            br.read_instruction(it.first.get_begin());
            std::cout << std::left << std::setw(24) << br.get_str() << "\t"
                      << it.second << std::endl;
        }
    }

   private:
    bytefile* bf;
    std::map<instruction, int32_t> codes;

    void calculate_codes() {
        const uint8_t* ip = bf->code_ptr;

        byte_reader br(eof, bf);
        while (ip < eof) {
            const uint8_t* insn_end = br.read_instruction(ip);
            instruction i(ip, insn_end);
            ip = insn_end;
            codes.try_emplace(i, 0).first->second++;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        failure("Empty input! Specify the path to the bytecode file!");
    }
    bytefile* bf = read_file(argv[1]);
    freq_counter x(bf);
    x.print_frequency();
    return 0;
}
