#include <trace.h>

void init_trace() {
    #ifdef CONFIG_ITRACE
        printf("test0\n");
        init_iringbuf();
    #endif

    #ifdef CONFIG_MTRACE
        init_mringbuf();
    #endif

    #ifdef CONFIG_DTRACE
        init_dringbuf();
    #endif

    #ifdef CONFIG_ETRACE
        init_eringbuf();
    #endif
}

void printf_trace(){
    #ifdef CONFIG_ITRACE
        printf_iringbuf();
    #endif

    #ifdef CONFIG_MTRACE
        printf_mringbuf();
    #endif

    #ifdef CONFIG_DTRACE
        printf_dringbuf();
    #endif

    if (is_use_ftrace) printf_fringbuf();
}