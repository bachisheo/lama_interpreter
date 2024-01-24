#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ios>

// end of bytefile
unsigned char* eof;

/*THE HELPING CODE FROM BYTERUN*/
/* The unpacked representation of bytecode file */
using bytefile = struct {
    char* string_ptr; /* A pointer to the beginning of the string table */
    int* public_ptr;  /* A pointer to the beginning of publics table    */
    unsigned char* code_ptr; /* A pointer to the bytecode itself */
    int* global_ptr;      /* A pointer to the global area                   */
    int stringtab_size;   /* The size (in bytes) of the string table        */
    int global_area_size; /* The size (in words) of global area             */
    int public_symbols_number; /* The number of public symbols */
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
    file->code_ptr = (unsigned char*)&file->string_ptr[file->stringtab_size];
    file->global_ptr = (int*)malloc(file->global_area_size * sizeof(int));

    return file;
}

// #include "../lama-v1.20/runtime/runtime.h"

#define STRING get_string(bf, next_int())
#define ASSERT_TRUE(condition, msg, ...)                         \
    do                                                           \
        if (!(condition)) failure("\n" msg "\n", ##__VA_ARGS__); \
    while (0)

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

struct freq_counter {
    freq_counter(bytefile* bf) : bf(bf) {}
    void print_frequency() {
        calculate_codes();
        std::vector<std::pair<std::string, int>> sorted_codes(codes.begin(),
                                                              codes.end());
        std::sort(sorted_codes.begin(), sorted_codes.end(), cmp);
        for (auto& it : sorted_codes) {
            std::cout << std::left << std::setw(24) << it.first << "\t"
                      << it.second << std::endl;
        }
    }

   private:
    unsigned char* ip;
    bytefile* bf;

    int32_t next_int() {
        ASSERT_TRUE(ip + sizeof(int) < eof, "IP points out of bytecode area!");
        return (ip += sizeof(int), *(int*)(ip - sizeof(int)));
    }
    unsigned char next_byte() {
        ASSERT_TRUE(ip < eof, "IP points out of bytecode area!");
        return *ip++;
    }

    static bool cmp(const std::pair<std::string, int>& a,
                    const std::pair<std::string, int>& b) {
        return a.second > b.second;
    }
    std::unordered_map<std::string, int32_t> codes;
    int h, l;
    const std::array<std::string, 13> ops = {
        "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
    const std::array<std::string, 7> pats = {
        "=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"};
    const std::array<std::string, 3> lds = {"LD", "LDA", "ST"};
    const std::array<std::string, 4> plcs = {"G", "L", "A", "C"};

    const std::string arg_sep = " ";

    const std::string sep = "\t";

    void inc(std::string key) {
        auto& entry = *(codes.try_emplace(key, 0).first);
        entry.second++;
    }

    void inc(std::string code, std::string args) {
        std::string key = code + sep + args;
        inc(key);
    }

    void inc(std::string code, int32_t arg) {
        std::string key = code + sep + std::to_string(arg);
        inc(key);
    }
    void inc(std::string code, int32_t arg1, int32_t arg2) {
        std::string key =
            code + sep + std::to_string(arg1) + std::to_string(arg2);
        inc(key);
    }

    void fail() { failure("ERROR: invalid opcode %d-%d\n", h, l); }

    template <typename T>
    std::string int_to_hex(T i) {
        std::stringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2)
               << std::hex << i;
        return stream.str();
    }
    std::string get_addr_view(char l, int32_t addr) {
        return "(" + plcs[l] + std::to_string(addr) + ")";
    }
    void calculate_codes() {
        ip = (unsigned char*)bf->code_ptr;
        do {
            char x = next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

            switch (h) {
                case 15:
                    return;

                /* BINOP */
                case 0: {
                    inc("BINOP", ops[l - 1]);
                    break;
                }

                case 1:
                    switch (l) {
                        case 0:
                            inc("CONST", next_int());
                            break;

                        case 1:
                            inc("STRING", STRING);
                            break;

                        case 2:
                            inc("SEXP",
                                STRING + arg_sep + std::to_string(next_int()));
                            break;

                        case 3:
                            inc("STI");
                            break;

                        case 4:
                            inc("STA");
                            break;

                        case 5:
                            inc("JMP", next_int());
                            break;

                        case 6:
                            inc("END");
                            break;

                        case 7:
                            inc("RET");
                            break;

                        case 8:
                            inc("DROP");
                            break;

                        case 9:
                            inc("DUP");
                            break;

                        case 10:
                            inc("SWAP");
                            break;

                        case 11:
                            inc("ELEM");
                            break;

                        default:
                            fail();
                    }
                    break;

                case 2:
                case 3:
                case 4: {
                    std::string place =
                        "(" + plcs[l] + std::to_string(next_int()) + ")";
                    inc(lds[h - 2], place);
                    break;
                }

                case 5:
                    switch (l) {
                        case 0:
                            inc("CJMPz", int_to_hex(next_int()) + ".8x");
                            break;

                        case 1:
                            inc("CJMPnz", int_to_hex(next_int()) + ".8x");
                            break;

                        case 2:
                            inc("BEGIN", next_int(), next_int());
                            break;

                        case 3:
                            inc("CBEGIN", next_int(), next_int());
                            break;

                        case 4: {
                            std::stringstream args;
                            args << next_int();
                            int n = next_int();
                            for (int i = 0; i < n; i++) {
                                unsigned char byte = next_byte();
                                args << arg_sep
                                     << get_addr_view(byte, next_int());
                            }
                            inc("CLOSURE", args.str());
                            break;
                        }

                        case 5:
                            inc("CALLC", next_int());
                            break;

                        case 6:
                            inc("CALL", next_int(), next_int());
                            break;

                        case 7:
                            inc("TAG ",
                                STRING + arg_sep + std::to_string(next_int()));
                            break;

                        case 8:
                            inc("ARRAY", next_int());
                            break;

                        case 9:
                            inc("FAIL", next_int(), next_int());
                            break;

                        case 10:
                            inc("LINE", next_int());
                            break;

                        default:
                            fail();
                    }
                    break;

                case 6:
                    inc("PATT", pats[l]);
                    break;

                case 7: {
                    std::string code = "CALL";
                    switch (l) {
                        case 0:
                            inc(code, "Lread");
                            break;

                        case 1:
                            inc(code, "Lwrite");
                            break;

                        case 2:
                            inc(code, "Llength");
                            break;

                        case 3:
                            inc(code, "Lstring");
                            break;

                        case 4:
                            inc(code, "Barray" + arg_sep +
                                          std::to_string(next_int()));
                            break;

                        default:
                            fail();
                    }
                } break;

                default:
                    fail();
            }

        } while (1);
    }
};

int main(int argc, char* argv[]) {
    bytefile* bf = read_file(argv[1]);
    freq_counter x(bf);
    x.print_frequency();
    return 0;
}
