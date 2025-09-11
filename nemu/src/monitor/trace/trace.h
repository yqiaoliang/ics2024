// #include "../sdb/sdb.h"
#define IRINGBUF_LEN 15


typedef struct{
    int iringbuf_len;
    char *iringbuf[IRINGBUF_LEN];
    int iringbuf_index;
    int iringbuf_full;
} Itrace;
void init_iringbuf();
void printf_iringbuf();
Itrace * get_itrace();


// struct Mtrace;
// void init_mringbuf();
// void printf_mringbuf();
// char * get_mringbuf();
// void update_mringbuf_idex(int idx, int is_full);
// int get_iringbuf_len();