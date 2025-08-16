#include "./test.h"
#include "stdio.h"
#include <stdbool.h>
#include "./sdb/sdb.h"
#include "../../tools/gen-expr/gen-expr.h"

int test(){
    for (int i = 0; i < 100; i++) {
        printf("test %d\n", i);
        char * this_expr = get_rand_expr();
        // char * this_expr = "(19+29)*(((35-72)))/83*66";
        // printf("expr: %s\n", this_expr);
        bool result = true;
        bool *success = &result;
        printf("%ld \n", expr(this_expr, success));
        printf("\n");
        
    }
    
    return 0;
}