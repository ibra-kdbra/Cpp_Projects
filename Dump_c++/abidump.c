#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include "thirdparty/linux/elf.h"
#include "thirdparty/tiny-regex-c/re.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Binary reader API
//

enum {
    READER_FLAG_NONE        = 0,
    READER_FLAG_SWAP_ENDIAN = 1 << 0,
    READER_FLAG_OVERFLOWED  = 1 << 1,
};

typedef struct {
    uint8_t*    bytes;
    size_t      length;
    size_t      cur;
    uint32_t    flags;
} reader_t;

static bool is_host_little_endian(void) {
    int n = 1;
    return *(char *)&n == 1;
}

static uint16_t swap16(uint16_t i) {
    return (i << 8) | (i >> 8);
}

static uint32_t swap32(uint32_t i) {
    i = ((i << 8) & 0xFF00FF00) | ((i >> 8) & 0xFF00FF);
    return (i << 16) | (i >> 16);
}

static uint64_t swap64(uint64_t i) {
    i = ((i << 8) & 0xFF00FF00FF00FF00ULL) | ((i >> 8) & 0x00FF00FF00FF00FFULL);
    i = ((i << 16) & 0xFFFF0000FFFF0000ULL) | ((i >> 16) & 0x0000FFFF0000FFFFULL);
    return (i << 32) | (i >> 32);
}

static bool reader_open(reader_t* reader, const char* path) {
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }
    fseek(f, 0, SEEK_END);
    reader->length = ftell(f);
    fseek(f, 0, SEEK_SET);
    reader->bytes = malloc(reader->length); assert(reader->bytes);
    fread(reader->bytes, reader->length, 1, f);
    fclose(f);
    return true;
}

static size_t reader_read(reader_t* reader, uint8_t* buf, size_t length) {
    size_t count = 0;
    for (; count < length; ++count) {
        if (reader->cur + count >= reader->length) {
            reader->flags |= READER_FLAG_OVERFLOWED;
            break;
        }
        buf[count] = reader->bytes[reader->cur];
        reader->cur += 1;
    }
    return count;
}

static uint8_t reader_u8(reader_t* reader) {
    if (reader->cur + sizeof(uint8_t) >= reader->length) {
        reader->flags |= READER_FLAG_OVERFLOWED;
        return 0;
    }
    uint8_t r = *(uint8_t*)&reader->bytes[reader->cur];
    reader->cur += sizeof(uint8_t);
    return r;
}

static uint16_t reader_u16(reader_t* reader) {
    if (reader->cur + sizeof(uint16_t) >= reader->length) {
        reader->flags |= READER_FLAG_OVERFLOWED;
        return 0;
    }
    uint16_t r = *(uint16_t*)&reader->bytes[reader->cur];
    if (reader->flags & READER_FLAG_SWAP_ENDIAN) {
        r = swap16(r);
    }
    reader->cur += sizeof(uint16_t);
    return r;
}

static uint32_t reader_u32(reader_t* reader) {
    if (reader->cur + sizeof(uint32_t) >= reader->length) {
        reader->flags |= READER_FLAG_OVERFLOWED;
        return 0;
    }
    uint32_t r = *(uint32_t*)&reader->bytes[reader->cur];
    if (reader->flags & READER_FLAG_SWAP_ENDIAN) {
        r = swap32(r);
    }
    reader->cur += sizeof(uint32_t);
    return r;
}

static uint64_t reader_u64(reader_t* reader) {
    if (reader->cur + sizeof(uint64_t) >= reader->length) {
        reader->flags |= READER_FLAG_OVERFLOWED;
        return 0;
    }
    uint64_t r = *(uint64_t*)&reader->bytes[reader->cur];
    if (reader->flags & READER_FLAG_SWAP_ENDIAN) {
        r = swap64(r);
    }
    reader->cur += sizeof(uint64_t);
    return r;
}

//
// ELF parser
//

enum {
    ELF_FLAG_NONE   = 0,
    ELF_FLAG_64BIT  = 1 << 0,
    ELF_FLAG_LE     = 1 << 1,   // little-endian
};

typedef struct {
    reader_t    reader;         // file reader
    uint16_t    flags;          // parser flags
    Elf64_Ehdr  ehdr;           // file header
    Elf64_Shdr* shdrs;          // section headers
    size_t      shdrs_count;
    Elf64_Sym*  syms;           // debug symbols
    size_t      syms_count;
    const char* strtab;         // string table
    size_t      strtab_length;
} elf_t;

#define READ_U8()  reader_u8(&elf->reader)
#define READ_U16() reader_u16(&elf->reader)
#define READ_U32() reader_u32(&elf->reader)
#define READ_U64() reader_u64(&elf->reader)
#define READ_PTR() (elf->flags & ELF_FLAG_64BIT) ? READ_U64() : READ_U32()

static bool elf_parse_ehdr(elf_t* elf, Elf64_Ehdr* ehdr) {
    reader_read(&elf->reader, ehdr->e_ident, EI_NIDENT);
    ehdr->e_type        = READ_U16();
    ehdr->e_machine     = READ_U16();
    ehdr->e_version     = READ_U32();
    ehdr->e_entry       = READ_PTR();
    ehdr->e_phoff       = READ_PTR();
    ehdr->e_shoff       = READ_PTR();
    ehdr->e_flags       = READ_U32();
    ehdr->e_ehsize      = READ_U16();
    ehdr->e_phentsize   = READ_U16();
    ehdr->e_phnum       = READ_U16();
    ehdr->e_shentsize   = READ_U16();
    ehdr->e_shnum       = READ_U16();
    ehdr->e_shstrndx    = READ_U16();
    return true;
}

static bool elf_parse_shdr(elf_t* elf, Elf64_Shdr* shdr) {
    shdr->sh_name       = READ_U32();
    shdr->sh_type       = READ_U32();
    shdr->sh_flags      = READ_PTR();
    shdr->sh_addr       = READ_PTR();
    shdr->sh_offset     = READ_PTR();
    shdr->sh_size       = READ_PTR();
    shdr->sh_link       = READ_U32();
    shdr->sh_info       = READ_U32();
    shdr->sh_addralign  = READ_PTR();
    shdr->sh_entsize    = READ_PTR();
    return true;
}

static bool elf_parse_sym(elf_t* elf, Elf64_Sym* sym) {
    if (elf->flags & ELF_FLAG_64BIT) {
        sym->st_name    = READ_U32();
        sym->st_info    = READ_U8();
        sym->st_other   = READ_U8();
        sym->st_shndx   = READ_U16();
        sym->st_value   = READ_U64();
        sym->st_size    = READ_U64();
    } else {
        sym->st_name    = READ_U32();
        sym->st_value   = READ_U32();
        sym->st_size    = READ_U32();
        sym->st_info    = READ_U8();
        sym->st_other   = READ_U8();
        sym->st_shndx   = READ_U16();
    }
    return true;
}

#undef READ_U8
#undef READ_U16
#undef READ_U32
#undef READ_U64
#undef READ_PTR

static Elf64_Shdr* elf_find_section(elf_t* elf, const char* name) {
    // Get section header string table
    if (elf->ehdr.e_shstrndx >= elf->shdrs_count) {
        return NULL;
    }
    Elf64_Shdr* shstrtabhdr = &elf->shdrs[elf->ehdr.e_shstrndx];
    if (shstrtabhdr->sh_offset >= elf->reader.length) {
        return NULL;
    }
    const char* shstrtab = (const char*)&elf->reader.bytes[shstrtabhdr->sh_offset];

    // Look up section
    for (size_t i = 0; i < elf->shdrs_count; ++i) {
        if (strcmp(&shstrtab[elf->shdrs[i].sh_name], name) == 0) {
            return &elf->shdrs[i];
        }
    }
    return NULL;
}

static Elf64_Sym* elf_find_sym_by_address(elf_t* elf, uint64_t addr) {
    for (size_t i = 0; i < elf->syms_count; ++i) {
        if (elf->syms[i].st_value == addr) {
            return &elf->syms[i];
        }
    }
    return NULL;
}

static bool elf_parse(elf_t* elf) {
    // Read identity bytes
    uint8_t ident[EI_NIDENT] = { 0 };
    reader_read(&elf->reader, ident, EI_NIDENT);

    // Check magic bytes
    if (memcmp(ident, ELFMAG, SELFMAG) != 0) {
        return false;
    }

    // Check 32/64 bit
    if (ident[EI_CLASS] == ELFCLASS64) {
        elf->flags |= ELF_FLAG_64BIT;
    }

    // Check endianness
    if (ident[EI_CLASS] == ELFDATA2LSB) {
        elf->flags |= ELF_FLAG_LE;
        if (!!(elf->flags & ELF_FLAG_LE) ^ is_host_little_endian()) {
            // layout differs, need to swap bytes as we read
            elf->reader.flags |= READER_FLAG_SWAP_ENDIAN;
        }
    }

    // Parse file header
    elf->reader.cur = 0;
    if (!elf_parse_ehdr(elf, &elf->ehdr)) {
        return false;
    }

    // Parse section headers
    elf->shdrs_count = elf->ehdr.e_shnum;
    elf->shdrs = malloc(sizeof(Elf64_Shdr) * elf->shdrs_count); assert(elf->shdrs);
    for (size_t i = 0; i < elf->shdrs_count; ++i) {
        elf->reader.cur = elf->ehdr.e_shoff + elf->ehdr.e_shentsize * i;
        if (!elf_parse_shdr(elf, &elf->shdrs[i])) {
            return false;
        }
    }

    // Store string table
    Elf64_Shdr* strtabhdr = elf_find_section(elf, ".strtab");
    if (strtabhdr != NULL) {
        if (strtabhdr->sh_offset >= elf->reader.length) {
            return false;
        }
        elf->strtab = (const char*)&elf->reader.bytes[strtabhdr->sh_offset];
        elf->strtab_length = strtabhdr->sh_size;
    }

    // Parse symbol table
    Elf64_Shdr* symtabhdr = elf_find_section(elf, ".symtab");
    if (symtabhdr != NULL) {
        elf->syms_count = symtabhdr->sh_size / symtabhdr->sh_entsize;
        elf->syms = malloc(sizeof(Elf64_Sym) * elf->syms_count); assert(elf->syms);
        for (size_t i = 0; i < elf->syms_count; ++i) {
            elf->reader.cur = symtabhdr->sh_offset + symtabhdr->sh_entsize * i;
            if (!elf_parse_sym(elf, &elf->syms[i])) {
                return false;
            }
            // pre-validate string table indices
            if (elf->syms[i].st_name >= elf->strtab_length) {
                return false;
            }
        }
    }

    return true;
}

//
// abidump code
//

char* abidump_demangle(const char* mangled);

const char* ABIDUMP_USAGE = 
    "abidump - Dump C++ Itanium ABI from ELF objects        \n"
    "                                                       \n"
    "USAGE:                                                 \n"
    "    abidump <file> <subcommand> [options]              \n"
    "                                                       \n"
    "SUBCOMMANDS:                                           \n"
    "    symbols [filter] [--show-mangled]                  \n"
    "        Dumps debug symbols. [filter] is an optional   \n"
    "        regex filter. If [--show-mangled] is set, the  \n"
    "        mangled symbol name will also be printed.      \n"
    "    vtables [filter] [--ids]                           \n"
    "       Dumps virtual function tables. [filter] is an   \n"
    "       optional regex filter. If [--ids] is set, table \n"
    "       indices will also be printed.                   \n";

static int abidump_symbols(elf_t* elf, int argc, char* argv[]) {
    // Parse options
    bool show_mangled = false;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--show-mangled") == 0) {
            show_mangled = true;
        } else if (argv[i][0] == '-') {
            printf("Unknown flag: %s\n", argv[i]);
            return 1;
        }
    }

    // Compile filter
    re_t filter = NULL;
    if (argc >= 1 && argv[0][0] != '-') {
        filter = re_compile(argv[0]);
    }

    for (size_t i = 0; i < elf->syms_count; ++i) {
        const Elf64_Sym* sym = &elf->syms[i];
        const char* mangled = &elf->strtab[sym->st_name];
        char* demangled = abidump_demangle(mangled);

        // Match filter
        if (filter != NULL) {
            int match_length = 0;
            const char* name = (demangled != NULL) ? demangled : mangled;
            if (re_matchp(filter, name, &match_length) == -1) {
                continue;
            }
        }

        // Print file offset
        printf("0x%016llX", sym->st_value);
        // Print mangled name
        if (show_mangled) {
            printf("    %s", mangled);
        }
        // Print demangled name
        if (demangled != NULL) {
            printf("    %s", demangled);
            free(demangled);
        } else {
            // print mangled name anyway if not already shown
            if (!show_mangled) {
                printf("    %s", mangled);
            }
        }

        printf("\n");
    }
    return 0;
}

static int abidump_vtables(elf_t* elf, int argc, char* argv[]) {
    // Parse options
    bool show_ids = false;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--ids") == 0) {
            show_ids = true;
        } else if (argv[i][0] == '-') {
            printf("Unknown flag: %s\n", argv[i]);
            return 1;
        }
    }

    // Compile filter
    re_t filter = NULL;
    if (argc >= 1 && argv[0][0] != '-') {
        filter = re_compile(argv[0]);
    }

    // Need .text section to validate table entries
    Elf64_Shdr* text = elf_find_section(elf, ".text");

    for (size_t i = 0; i < elf->syms_count; ++i) {
        const Elf64_Sym* sym = &elf->syms[i];
        const char* mangled = &elf->strtab[sym->st_name];

        // Ignore anything that isn't a vtable
        if (strncmp(mangled, "_ZTV", 4) != 0) {
            continue;
        }

        char* demangled = abidump_demangle(mangled);
        if (demangled == NULL) {
            continue;
        }
        // skip "vtable for "
        const char* name = &demangled[11];

        // Match filter
        if (filter != NULL) {
            int match_length = 0;
            if (re_matchp(filter, name, &match_length) == -1) {
                continue;
            }
        }

        printf("VTable for %s:\n", name);

        // Walk table entries
        size_t ptr_size = (elf->flags & ELF_FLAG_64BIT) ? 8 : 4;
        size_t offset = ptr_size * 3;
        for (size_t i = 0; ; ++i) {
            elf->reader.cur = sym->st_value + offset + ptr_size * i;
            // Read function pointer
            uint64_t faddr = (elf->flags & ELF_FLAG_64BIT) ?
                reader_u64(&elf->reader) :
                reader_u32(&elf->reader);
            // Make sure faddr points to a code address
            if (faddr < text->sh_offset || faddr >= text->sh_offset + text->sh_size) {
                break;
            }
            // Get the symbol for this function
            Elf64_Sym* fsym = NULL;
            if ((fsym = elf_find_sym_by_address(elf, faddr)) == NULL) {
                break;
            }
            char* fname = NULL;
            if ((fname = abidump_demangle(&elf->strtab[fsym->st_name])) == NULL) {
                break;
            }

            printf("    ");
            if (show_ids) {
                printf("[%zu] ", i);
            }
            printf("%s\n", fname);

            free(fname);
        }
        free(demangled);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("%s", ABIDUMP_USAGE);
        return 1;
    }

    elf_t elf = { 0 };
    // Open file
    if (!reader_open(&elf.reader, argv[1])) {
        printf("Failed to open file \"%s\"\n", argv[1]);
        return 1;
    }

    // Parse
    if (!elf_parse(&elf)) {
        printf("\"%s\" is not a valid ELF object\n", argv[1]);
        return 1;
    }

    // Run subcommand
    if (strcmp(argv[2], "symbols") == 0) {
        return abidump_symbols(&elf, argc - 3, &argv[3]);
    } else if (strcmp(argv[2], "vtables") == 0) {
        return abidump_vtables(&elf, argc - 3, &argv[3]);
    } else {
        printf("Unknown subcommand: %s\n", argv[2]);
        printf("%s", ABIDUMP_USAGE);
        return 1;
    }
    return 0;
}