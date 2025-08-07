#include "./test.h"
#include "stdio.h"
#include <stdbool.h>
#include "./sdb/sdb.h"
#include "../../tools/gen-expr/gen-expr.h"

int test(){
    
    char * this_expr = get_rand_expr();
    bool result = true;
    bool *success = &result;
    printf("%d \n", expr(this_expr, success));
    return 0;
}