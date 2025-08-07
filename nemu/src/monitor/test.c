#include "./test.h"
#include "stdio.h"
#include <stdbool.h>
#include "./sdb/sdb.h"
#include "../../tools/gen-expr/gen-expr.h"

int test(){
    for (int i = 0; i < 10; i++) {
        printf("test %d\n", i);
        char * this_expr = get_rand_expr();
        printf("expr: %s\n", this_expr);
        bool result = true;
        bool *success = &result;
        printf("%d \n", expr(this_expr, success));
        
    }
    
    return 0;
}