#include "trace.h"

void printf_trace(){
    #ifdef CONFIG_ITRACE
        printf_iringbuf();
    #endif

    #ifdef CONFIG_MTRACE
        printf_mringbuf();
    #endif

    if (is_use_ftrace) printf_fringbuf();
}