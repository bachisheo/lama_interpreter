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

// #include "../lama-v1.20/runtime/runtime.h"

#define STRING std::string(get_string(bf, next_int()))
#define ASSERT_TRUE(condition, msg, ...)                         \
    do                                                           \
        if (!(condition)) failure("\n" msg "\n", ##__VA_ARGS__); \
    while (0)

/* Gets a string from a string table by an index */
const char* get_string(bytefile* f, int pos) {
    ASSERT_TRUE(pos >= 0 && pos < f->stringtab_size,
                "Index out of string pool!");
    return &f->string_ptr[pos];
}

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
    uint8_t* ip;
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
            code + sep + std::to_string(arg1) + arg_sep + std::to_string(arg2);
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
        return plcs[l] + "(" + std::to_string(addr) + ")";
    }
    void calculate_codes() {
        ip = (uint8_t*)bf->code_ptr;
        do {
            uint8_t x = next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

            // outer switch
            enum { BINOP, H1_OPS, LD, LDA, ST, H5_OPS, PATT, H7_OPS };
            // H1 OPS
            enum {
                CONST,
                BSTRING,
                BSEXP,
                STI,
                STA,
                JMP,
                END,
                RET,
                DROP,
                DUP,
                SWAP,
                ELEM
            };
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

            switch (h) {
                case 15:
                    return;

                case BINOP: {
                    inc("BINOP", ops[l - 1]);
                    break;
                }

                case H1_OPS:
                    switch (l) {
                        case CONST:
                            inc("CONST", next_int());
                            break;

                        case BSTRING:
                            inc("STRING", STRING);
                            break;

                        case BSEXP: {
                            std::string name = STRING;
                            int32_t args = next_int();
                            inc("SEXP ", name + arg_sep + std::to_string(args));
                            break;
                        }

                        case STI:
                            inc("STI");
                            break;

                        case STA:
                            inc("STA");
                            break;

                        case JMP:
                            inc("JMP", int_to_hex(next_int()));
                            break;

                        case END:
                            inc("END");
                            break;

                        case RET:
                            inc("RET");
                            break;

                        case DROP:
                            inc("DROP");
                            break;

                        case DUP:
                            inc("DUP");
                            break;

                        case SWAP:
                            inc("SWAP");
                            break;

                        case ELEM:
                            inc("ELEM");
                            break;

                        default:
                            fail();
                    }
                    break;

                case LD:
                case LDA:
                case ST:
                    inc(lds[h - 2], get_addr_view(l, next_int()));
                    break;

                case H5_OPS:
                    switch (l) {
                        case CJMPZ:
                            inc("CJMPz", int_to_hex(next_int()));
                            break;

                        case CJMPNZ:
                            inc("CJMPnz", int_to_hex(next_int()));
                            break;

                        case BEGIN: {
                            int n_args = next_int();
                            int n_locs = next_int();
                            inc("BEGIN", n_args, n_locs);
                            break;
                        }

                        case CBEGIN:
                            inc("CBEGIN", next_int(), next_int());
                            break;

                        case BCLOSURE: {
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

                        case CALLC:
                            inc("CALLC", next_int());
                            break;

                        case CALL: {
                            std::string addr = int_to_hex(next_int());
                            inc("CALL",
                                addr + arg_sep + std::to_string(next_int()));
                            break;
                        }

                        case TAG: {
                            std::string name = STRING;
                            int32_t args = next_int();
                            inc("TAG ", name + arg_sep + std::to_string(args));
                            break;
                        }

                        case ARRAY_KEY:
                            inc("ARRAY", next_int());
                            break;

                        case FAIL: {
                            int line = next_int();
                            int col = next_int();
                            inc("FAIL", line, col);
                            break;
                        }

                        case LINE:
                            inc("LINE", next_int());
                            break;

                        default:
                            fail();
                    }
                    break;

                case PATT:
                    inc("PATT", pats[l]);
                    break;

                case H7_OPS: {
                    std::string code = "CALL";
                    switch (l) {
                        case LREAD:
                            inc(code, "Lread");
                            break;

                        case LWRITE:
                            inc(code, "Lwrite");
                            break;

                        case LLENGTH:
                            inc(code, "Llength");
                            break;

                        case LSTRING:
                            inc(code, "Lstring");
                            break;

                        case BARRAY:
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
    if (argc < 2) {
        failure("Empty input! Specify the path to the bytecode file!");
    }
    bytefile* bf = read_file(argv[1]);
    freq_counter x(bf);
    x.print_frequency();
    return 0;
}
