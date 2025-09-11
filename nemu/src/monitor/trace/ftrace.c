#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include "trace.h"



// 全局变量：存储所有FUNC类型符号
static FuncSymbol *func_symbols = NULL;
static int func_sym_count = 0;

// 函数声明
void *map_elf_file(const char *filename, size_t *file_size);
int validate_elf_header(void *elf_data);
Elf32_Shdr *find_symbol_table(void *elf_data);
void parse_symbol_table(void *elf_data, Elf32_Shdr *symtab_shdr);
void print_symbol_info(Elf32_Sym *symbol, const char *sym_name);
int init_ftrace(char *file_path);
const FuncSymbol *find_func_by_instr_addr(uint32_t instr_addr);

// 映射ELF文件到内存
void *map_elf_file(const char *filename, size_t *file_size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("ERROR: can't open %s file: %s\n", filename, strerror(errno));
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        printf("ERROR: can't get stats from %s file: %s\n", filename, strerror(errno));
        close(fd);
        return NULL;
    }
    *file_size = st.st_size;

    void *elf_data = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf_data == MAP_FAILED) {
        printf("ERROR: can't map %s file: %s\n", filename, strerror(errno));
        close(fd);
        return NULL;
    }

    close(fd);
    return elf_data;
}

// 验证ELF文件头部
int validate_elf_header(void *elf_data) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
    
    // 检查ELF魔数
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        printf("ERROR: not an ELF file\n");
        return 0;
    }
    
    // 检查是否为32位ELF
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("ERROR: only 32-bit ELF files are supported\n");
        return 0;
    }
    
    return 1;
}

// 查找符号表节
Elf32_Shdr *find_symbol_table(void *elf_data) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(elf_data + ehdr->e_shoff);
    int strtab_idx = ehdr->e_shstrndx;
    const char *strtab = (const char *)(elf_data + shdr[strtab_idx].sh_offset);

    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *section_name = strtab + shdr[i].sh_name;
        if (strcmp(section_name, ".symtab") == 0) {
            return &shdr[i];
        }
    }
    return NULL;
}

// 解析符号表（提取并存储FUNC类型符号）
void parse_symbol_table(void *elf_data, Elf32_Shdr *symtab_shdr) {
    // 释放旧的符号列表（如果存在）
    if (func_symbols != NULL) {
        free(func_symbols);
        func_symbols = NULL;
        func_sym_count = 0;
    }

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(elf_data + ehdr->e_shoff);
    Elf32_Sym *symbols = (Elf32_Sym *)(elf_data + symtab_shdr->sh_offset);
    int num_symbols = symtab_shdr->sh_size / sizeof(Elf32_Sym);

    // 获取符号字符串表
    Elf32_Shdr *strtab_shdr = &shdr[symtab_shdr->sh_link];
    const char *strtab = (const char *)(elf_data + strtab_shdr->sh_offset);

    // 第一步：统计FUNC类型符号数量
    for (int i = 0; i < num_symbols; i++) {
        if (ELF32_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            func_sym_count++;
        }
    }

    // 分配内存存储FUNC符号
    if (func_sym_count > 0) {
        func_symbols = malloc(func_sym_count * sizeof(FuncSymbol));
        if (func_symbols == NULL) {
            fprintf(stderr, "ERROR: malloc failed for func_symbols\n");
            func_sym_count = 0;
            return;
        }
    }

    // 第二步：填充FUNC符号列表并打印所有符号
    int func_idx = 0;
    for (int i = 0; i < num_symbols; i++) {
        const char *sym_name = strtab + symbols[i].st_name;
        if (sym_name[0] == '\0' && i != 0) continue;
        
        // printf("symbol #%d: ", i);
        // print_symbol_info(&symbols[i], sym_name);

        // 保存FUNC类型符号
        if (ELF32_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            func_symbols[func_idx] = (FuncSymbol){
                .name = sym_name,
                .addr = symbols[i].st_value,
                .size = symbols[i].st_size
            };
            func_idx++;
        }
    }
}

// 打印单个符号信息
void print_symbol_info(Elf32_Sym *symbol, const char *sym_name) {
    // 符号类型
    const char *type;
    switch (ELF32_ST_TYPE(symbol->st_info)) {
        case STT_NOTYPE:  type = "未定义类型"; break;
        case STT_OBJECT:  type = "数据对象"; break;
        case STT_FUNC:    type = "函数"; break;
        case STT_SECTION: type = "节"; break;
        case STT_FILE:    type = "文件名"; break;
        default:          type = "未知类型";
    }

    // 符号绑定
    const char *bind;
    switch (ELF32_ST_BIND(symbol->st_info)) {
        case STB_LOCAL:  bind = "局部"; break;
        case STB_GLOBAL: bind = "全局"; break;
        case STB_WEAK:   bind = "弱引用"; break;
        default:         bind = "未知绑定";
    }

    printf("名称: %-20s 地址: 0x%08x 长度: %-4d 类型: %-8s 绑定: %s\n",
           sym_name,
           (unsigned int)symbol->st_value,
           (int)symbol->st_size,
           type,
           bind);
}

// 初始化ftrace（解析ELF文件并加载符号表）
int init_ftrace(char *file_path) {
    size_t file_size;
    void *elf_data = map_elf_file(file_path, &file_size);
    if (elf_data == NULL) {
        return EXIT_FAILURE;
    }

    // 验证ELF文件头部
    if (!validate_elf_header(elf_data)) {
        fprintf(stderr, "ERROR: invalid ELF file\n");
        munmap(elf_data, file_size);
        return EXIT_FAILURE;
    }

    // 查找符号表
    Elf32_Shdr *symtab_shdr = find_symbol_table(elf_data);
    if (symtab_shdr == NULL) {
        fprintf(stderr, "ERROR: .symtab section not found\n");
        munmap(elf_data, file_size);
        return EXIT_FAILURE;
    }

    // 解析并打印符号表（同时存储FUNC符号）
    printf("Successfully loaded symbol table, total %ld symbols:\n",
           symtab_shdr->sh_size / sizeof(Elf32_Sym));
    printf("-------------------------------------------------\n");
    parse_symbol_table(elf_data, symtab_shdr);

    // 清理资源
    munmap(elf_data, file_size);
    return EXIT_SUCCESS;
}

/**
 * API: 根据指令地址查询所属函数
 * @param instr_addr 指令的内存地址
 * @return 函数符号指针（找到则返回，否则返回NULL）
 *         函数符号包含名称、起始地址和大小
 */
const FuncSymbol *find_func_by_instr_addr(uint32_t instr_addr) {
    // 遍历所有函数符号，检查地址是否在函数范围内
    for (int i = 0; i < func_sym_count; i++) {
        const FuncSymbol *func = &func_symbols[i];
        // 函数地址范围：[addr, addr + size)
        if (instr_addr >= func->addr && instr_addr < (func->addr + func->size)) {
            return func;
        }
    }
    return NULL;  // 未找到对应函数
}

