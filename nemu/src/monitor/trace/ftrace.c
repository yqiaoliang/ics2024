#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void *map_elf_file(const char *filename, size_t *file_size);
int validate_elf_header(void *elf_data);
Elf32_Shdr *find_symbol_table(void *elf_data);
void parse_symbol_table(void *elf_data, Elf32_Shdr *symtab_shdr);
void print_symbol_info(Elf32_Sym *symbol, const char *sym_name);


void *map_elf_file(const char *filename, size_t *file_size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("ERROR: can't open %s file", filename);
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        printf("ERROR: can't grep data from %s file", filename);
        close(fd);
        return NULL;
    }
    *file_size = st.st_size;

    void *elf_data = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf_data == MAP_FAILED) {
        printf("ERROR: cant't map file of %s", filename);
        close(fd);
        return NULL;
    }

    close(fd);
    return elf_data;
}

int validate_elf_header(void *elf_data) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
    
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        return 0;
    }
    
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("ERROR: only support 32 bit");
        return 0;
    }
    
    return 1;
}

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

void parse_symbol_table(void *elf_data, Elf32_Shdr *symtab_shdr) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(elf_data + ehdr->e_shoff);
    Elf32_Sym *symbols = (Elf32_Sym *)(elf_data + symtab_shdr->sh_offset);
    int num_symbols = symtab_shdr->sh_size / sizeof(Elf32_Sym);

    Elf32_Shdr *strtab_shdr = &shdr[symtab_shdr->sh_link];
    const char *strtab = (const char *)(elf_data + strtab_shdr->sh_offset);

    for (int i = 0; i < num_symbols; i++) {
        const char *sym_name = strtab + symbols[i].st_name;
        if (sym_name[0] == '\0' && i != 0) continue;
        
        printf("符号 #%d: ", i);
        print_symbol_info(&symbols[i], sym_name);
    }
}

int init_ftrace(char * file_path){
    size_t file_size;
    void *elf_data = map_elf_file(file_path, &file_size);
    if (elf_data == NULL) {
        return EXIT_FAILURE;
    }

    // 验证ELF文件头部
    if (!validate_elf_header(elf_data)) {
        fprintf(stderr, "错误: 不是有效的ELF文件\n");
        munmap(elf_data, file_size);
        return EXIT_FAILURE;
    }

    // 查找符号表
    Elf32_Shdr *symtab_shdr = find_symbol_table(elf_data);
    if (symtab_shdr == NULL) {
        fprintf(stderr, "错误: 未找到符号表(.symtab)\n");
        munmap(elf_data, file_size);
        return EXIT_FAILURE;
    }

    // 解析并打印符号表
    printf("成功找到符号表，共 %ld 个符号:\n", 
           symtab_shdr->sh_size / sizeof(Elf32_Sym));
    printf("-------------------------------------------------\n");
    parse_symbol_table(elf_data, symtab_shdr);

    // 清理资源
    munmap(elf_data, file_size);
    return EXIT_SUCCESS;
}

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
