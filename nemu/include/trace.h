#include <stdint.h>

#define IRINGBUF_LEN 15
#define MRINGBUF_LEN 20
#define FRINGBUF_LEN 40
#define DRINGBUF_LEN 20
#define ERINGBUF_LEN 20

extern int is_use_ftrace;

// ITRACE
typedef struct{
    int iringbuf_len;
    char *iringbuf[IRINGBUF_LEN];
    int iringbuf_index;
    int iringbuf_full;
} Itrace;
void init_iringbuf();
void printf_iringbuf();
Itrace * get_itrace();



// MTRACE
typedef struct{
    int mringbuf_len;
    char *mringbuf[MRINGBUF_LEN];
    int mringbuf_index;
    int mringbuf_full;
} Mtrace;
void init_mringbuf();
void printf_mringbuf();
Mtrace * get_mtrace();



// TTRACE
typedef struct {
    const char *name;       // func name
    uint32_t addr;          // func start addr
    uint32_t size;          // size of func
} FuncSymbol;

typedef struct{
    int fringbuf_len;
    char *fringbuf[FRINGBUF_LEN];
    int fringbuf_index;
    int fringbuf_full;
} Ftrace;
int init_ftrace(char * file_path);
void printf_fringbuf();
Ftrace * get_ftrace();
const FuncSymbol *find_func_by_instr_addr(uint32_t instr_addr);
void cleanup_ftrace(void);


typedef struct{
    int dringbuf_len;
    char *dringbuf[DRINGBUF_LEN];
    int dringbuf_index;
    int dringbuf_full;
} Dtrace;
void init_dringbuf();
void printf_dringbuf();
Dtrace * get_dtrace();



typedef struct{
    int eringbuf_len;
    char *eringbuf[ERINGBUF_LEN];
    int eringbuf_index;
    int eringbuf_full;
} Etrace;
void init_eringbuf();
void printf_eringbuf();
Etrace * get_etrace();



void printf_trace();