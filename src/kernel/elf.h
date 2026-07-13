
#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>
#include <stdbool.h>

#define ELF32_ST_BIND(i)   ((i) >> 4)
#define ELF32_ST_TYPE(i)   ((i) & 0xf)
#define ELF32_ST_INFO(b,t) (((b) << 4) + ((t) & 0xf))

#define ELF64_ST_BIND(i)   ELF32_ST_BIND(i)
#define ELF64_ST_TYPE(i)   ELF32_ST_TYPE(i)
#define ELF64_ST_INFO(b,t) ELF32_ST_INFO(b,t)

#define ELF32_R_SYM(i)   ((i) >> 8)
#define ELF32_R_TYPE(i)  ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s) << 8) + (unsigned char)(t))

#define ELF64_R_SYM(i)   ((i) >> 32)
#define ELF64_R_TYPE(i)  ((uint32_t)(i))
#define ELF64_R_INFO(s,t) (((uint64_t)(s) << 32) + (uint32_t)(t))

typedef uint8_t  Elf_Byte;

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;
typedef int64_t  Elf32_Sxword;
typedef uint64_t Elf32_Xword;

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef int32_t  Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Xword;

typedef Elf32_Half Elf32_Section;
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Section;
typedef Elf64_Half Elf64_Versym;

#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

typedef enum
{
        DT_NULL            = 0,
        DT_NEEDED          = 1,
        DT_PLTRELSZ        = 2,
        DT_PLTGOT          = 3,
        DT_HASH            = 4,
        DT_STRTAB          = 5,
        DT_SYMTAB          = 6,
        DT_RELA            = 7,
        DT_RELASZ          = 8,
        DT_RELAENT         = 9,
        DT_STRSZ           = 10,
        DT_SYMENT          = 11,
        DT_INIT            = 12,
        DT_FINI            = 13,
        DT_SONAME          = 14,
        DT_RPATH           = 15,
        DT_SYMBOLIC        = 16,
        DT_REL             = 17,
        DT_RELSZ           = 18,
        DT_RELENT          = 19,
        DT_PLTREL          = 20,
        DT_DEBUG           = 21,
        DT_TEXTREL         = 22,
        DT_JMPREL          = 23,
        DT_BIND_NOW        = 24,
        DT_INIT_ARRAY      = 25,
        DT_FINI_ARRAY      = 26,
        DT_INIT_ARRAYSZ    = 27,
        DT_FINI_ARRAYSZ    = 28,
        DT_RUNPATH         = 29,
        DT_FLAGS           = 30,
        DT_PREINIT_ARRAY   = 32,
        DT_PREINIT_ARRAYSZ = 33,
} DT_T;

typedef enum
{
        STT_NOTYPE  = 0,
        STT_OBJECT  = 1,
        STT_FUNC    = 2,
        STT_SECTION = 3,
        STT_FILE    = 4,
        STT_COMMON  = 5,
        STT_TLS     = 6,
        STT_LOOS    = 10,
        STT_HIOS    = 12,
        STT_LOPROC  = 13,
        STT_HIPROC  = 15,
} STT_T;

typedef enum
{
        STB_LOCAL  = 0,
        STB_GLOBAL = 1,
        STB_WEAK   = 2,
        STB_LOPROC = 13,
        STB_HIPROC = 15,
} STB_T;

typedef enum
{
        SHN_UNDEF     = 0x0000,
        SHN_LORESERVE = 0xff00,
        SHN_LOPROC    = 0xff00,
        SHN_HIPROC    = 0xff1f,
        SHN_LOOS      = 0xff20,
        SHN_HIOS      = 0xff3f,
        SHN_ABS       = 0xfff1,
        SHN_COMMON    = 0xfff2,
        SHN_XINDEX    = 0xffff,
        SHN_HIRESERVE = 0xffff,
} SHN_T;

typedef enum
{
        SHT_NULL          = 0,
        SHT_PROGBITS      = 1,
        SHT_SYMTAB        = 2,
        SHT_STRTAB        = 3,
        SHT_RELA          = 4,
        SHT_HASH          = 5,
        SHT_DYNAMIC       = 6,
        SHT_NOTE          = 7,
        SHT_NOBITS        = 8,
        SHT_REL           = 9,
        SHT_SHLIB         = 10,
        SHT_DYNSYM        = 11,
        SHT_INIT_ARRAY    = 14,
        SHT_FINI_ARRAY    = 15,
        SHT_PREINIT_ARRAY = 16,
        SHT_GROUP         = 17,
        SHT_SYMTAB_SHNDX  = 18,
        SHT_NUM           = 19,
        SHT_LOOS          = 0x60000000,
        SHT_HIOS          = 0x6fffffff,
        SHT_LOPROC        = 0x70000000,
        SHT_HIPROC        = 0x7fffffff,
        SHT_LOUSER        = (int)0x80000000,
        SHT_HIUSER        = (int)0xffffffff,
} SHT_T;

typedef enum
{
        SHF_WRITE            = 0x1,
        SHF_ALLOC            = 0x2,
        SHF_EXECINSTR        = 0x4,
        SHF_MERGE            = 0x10,
        SHF_STRINGS          = 0x20,
        SHF_INFO_LINK        = 0x40,
        SHF_LINK_ORDER       = 0x80,
        SHF_OS_NONCONFORMING = 0x100,
        SHF_GROUP            = 0x200,
        SHF_TLS              = 0x400,
        SHF_MASKOS           = 0x0ff00000,
        SHF_MASKPROC         = (int)0xf0000000,
} SHF_T;

typedef enum
{
        ELFCLASSNONE = 0,
        ELFCLASS32   = 1,
        ELFCLASS64   = 2,
} ELFCLASS_T;

typedef enum
{
        ELFDATANONE = 0,
        ELFDATA2LSB = 1,
        ELFDATA2MSB = 2,
} ELFDATA_T;

typedef enum
{
        ELFOSABI_NONE       = 0,
        ELFOSABI_SYSV       = 0,
        ELFOSABI_HPUX       = 1,
        ELFOSABI_NETBSD     = 2,
        ELFOSABI_LINUX      = 3,
        ELFOSABI_SOLARIS    = 6,
        ELFOSABI_AIX        = 7,
        ELFOSABI_IRIX       = 8,
        ELFOSABI_FREEBSD    = 9,
        ELFOSABI_TRU64      = 10,
        ELFOSABI_MODESTO    = 11,
        ELFOSABI_OPENBSD    = 12,
        ELFOSABI_ARM        = 97,
        ELFOSABI_STANDALONE = 255,
} ELFOSABI_T;

typedef enum
{
        ET_NONE   = 0,
        ET_REL    = 1,
        ET_EXEC   = 2,
        ET_DYN    = 3,
        ET_CORE   = 4,
        ET_LOOS   = 0xfe00,
        ET_HIOS   = 0xfeff,
        ET_LOPROC = 0xff00,
        ET_HIPROC = 0xffff,
} ET_T;

typedef enum
{
        EM_NONE        = 0,
        EM_M32         = 1,
        EM_SPARC       = 2,
        EM_386         = 3,
        EM_68K         = 4,
        EM_88K         = 5,
        EM_860         = 7,
        EM_MIPS        = 8,
        EM_PARISC      = 15,
        EM_SPARC32PLUS = 18,
        EM_PPC         = 20,
        EM_PPC64       = 21,
        EM_S390        = 22,
        EM_ARM         = 40,
        EM_SH          = 42,
        EM_SPARCV9     = 43,
        EM_IA_64       = 50,
        EM_X86_64      = 62,
        EM_AARCH64     = 183,
        EM_RISCV       = 243,
} EM_T;

typedef enum
{
        EV_NONE    = 0,
        EV_CURRENT = 1,
} EV_T;

typedef enum
{
        EI_MAG0       = 0,
        EI_MAG1       = 1,
        EI_MAG2       = 2,
        EI_MAG3       = 3,
        EI_CLASS      = 4,
        EI_DATA       = 5,
        EI_VERSION    = 6,
        EI_OSABI      = 7,
        EI_ABIVERSION = 8,
        EI_PAD        = 9,
} EI_OFF_T;

typedef enum
{
        PT_NULL         = 0,
        PT_LOAD         = 1,
        PT_DYNAMIC      = 2,
        PT_INTERP       = 3,
        PT_NOTE         = 4,
        PT_SHLIB        = 5,
        PT_PHDR         = 6,
        PT_TLS          = 7,
        PT_LOOS         = 0x60000000,
        PT_HIOS         = 0x6fffffff,
        PT_LOPROC       = 0x70000000,
        PT_HIPROC       = 0x7fffffff,
        PT_GNU_EH_FRAME = 0x6474e550,
        PT_GNU_STACK    = 0x6474e551,
        PT_GNU_RELRO    = 0x6474e552,
} PT_T;

typedef enum
{
        PF_X = 0x1,
        PF_W = 0x2,
        PF_R = 0x4,
} PF_T;

typedef struct
{
        Elf_Byte    e_ident[EI_NIDENT];
        Elf32_Half  e_type;
        Elf32_Half  e_machine;
        Elf32_Word  e_version;
        Elf32_Addr  e_entry;
        Elf32_Off   e_phoff;
        Elf32_Off   e_shoff;
        Elf32_Word  e_flags;
        Elf32_Half  e_ehsize;
        Elf32_Half  e_phentsize;
        Elf32_Half  e_phnum;
        Elf32_Half  e_shentsize;
        Elf32_Half  e_shnum;
        Elf32_Half  e_shstrndx;
} Elf32_Ehdr;

typedef struct
{
        Elf_Byte    e_ident[EI_NIDENT];
        Elf64_Half  e_type;
        Elf64_Half  e_machine;
        Elf64_Word  e_version;
        Elf64_Addr  e_entry;
        Elf64_Off   e_phoff;
        Elf64_Off   e_shoff;
        Elf64_Word  e_flags;
        Elf64_Half  e_ehsize;
        Elf64_Half  e_phentsize;
        Elf64_Half  e_phnum;
        Elf64_Half  e_shentsize;
        Elf64_Half  e_shnum;
        Elf64_Half  e_shstrndx;
} Elf64_Ehdr;

typedef struct
{
        Elf32_Word  p_type;
        Elf32_Off   p_offset;
        Elf32_Addr  p_vaddr;
        Elf32_Addr  p_paddr;
        Elf32_Word  p_filesz;
        Elf32_Word  p_memsz;
        Elf32_Word  p_flags;
        Elf32_Word  p_align;
} Elf32_Phdr;

typedef struct
{
        Elf64_Word  p_type;
        Elf64_Word  p_flags;
        Elf64_Off   p_offset;
        Elf64_Addr  p_vaddr;
        Elf64_Addr  p_paddr;
        Elf64_Xword p_filesz;
        Elf64_Xword p_memsz;
        Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct
{
        Elf32_Word  sh_name;
        Elf32_Word  sh_type;
        Elf32_Word  sh_flags;
        Elf32_Addr  sh_addr;
        Elf32_Off   sh_offset;
        Elf32_Word  sh_size;
        Elf32_Word  sh_link;
        Elf32_Word  sh_info;
        Elf32_Word  sh_addralign;
        Elf32_Word  sh_entsize;
} Elf32_Shdr;

typedef struct
{
        Elf64_Word  sh_name;
        Elf64_Word  sh_type;
        Elf64_Xword sh_flags;
        Elf64_Addr  sh_addr;
        Elf64_Off   sh_offset;
        Elf64_Xword sh_size;
        Elf64_Word  sh_link;
        Elf64_Word  sh_info;
        Elf64_Xword sh_addralign;
        Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct
{
        Elf32_Word  st_name;
        Elf32_Addr  st_value;
        Elf32_Word  st_size;
        Elf_Byte    st_info;
        Elf_Byte    st_other;
        Elf32_Half  st_shndx;
} Elf32_Sym;

typedef struct
{
        Elf64_Word  st_name;
        Elf_Byte    st_info;
        Elf_Byte    st_other;
        Elf64_Half  st_shndx;
        Elf64_Addr  st_value;
        Elf64_Xword st_size;
} Elf64_Sym;

typedef struct
{
        Elf32_Addr r_offset;
        Elf32_Word r_info;
} Elf32_Rel;

typedef struct
{
        Elf32_Addr  r_offset;
        Elf32_Word  r_info;
        Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct
{
        Elf64_Addr  r_offset;
        Elf64_Xword r_info;
} Elf64_Rel;

typedef struct
{
        Elf64_Addr   r_offset;
        Elf64_Xword  r_info;
        Elf64_Sxword r_addend;
} Elf64_Rela;

typedef union
{
        Elf32_Word  d_val;
        Elf32_Addr  d_ptr;
} Elf32_Dyn_Val;

typedef struct
{
        Elf32_Sword   d_tag;
        Elf32_Dyn_Val d_un;
} Elf32_Dyn;

typedef union
{
        Elf64_Xword d_val;
        Elf64_Addr  d_ptr;
} Elf64_Dyn_Val;

typedef struct
{
        Elf64_Sxword  d_tag;
        Elf64_Dyn_Val d_un;
} Elf64_Dyn;

typedef struct
{
        Elf32_Word n_namesz;
        Elf32_Word n_descsz;
        Elf32_Word n_type;
} Elf32_Nhdr;

typedef struct
{
        Elf64_Word n_namesz;
        Elf64_Word n_descsz;
        Elf64_Word n_type;
} Elf64_Nhdr;

#include <cpu/elf.h> // elf info like what are we

#if ELF_CLASS == ELFCLASS32

typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;

#elif ELF_CLASS == ELFCLASS64

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;

#endif

#include <mm/paging.h>
#include <fs/fs.h>

int elf_load(file_t *file, address_space_t *space, Elf_Addr *entry);

#endif
