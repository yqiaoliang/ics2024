// #include "../sdb/sdb.h"
#define IRINGBUF_LEN 15
#define MRINGBUF_LEN 20


typedef struct{
    int iringbuf_len;
    char *iringbuf[IRINGBUF_LEN];
    int iringbuf_index;
    int iringbuf_full;
} Itrace;
void init_iringbuf();
void printf_iringbuf();
Itrace * get_itrace();


typedef struct{
    int mringbuf_len;
    char *mringbuf[MRINGBUF_LEN];
    int mringbuf_index;
    int mringbuf_full;
} Mtrace;
void init_mringbuf();
void printf_mringbuf();
Mtrace * get_mtrace();