#include "./test.h"
#include "stdio.h"
#include <stdbool.h>
#include "./sdb/sdb.h"

int test(){
    
    char * this_expr = "2 + 3 * 5 -(8 * 9)";
    bool result = true;
    bool *success = &result;
    printf("%d \n", expr(this_expr, success));
    return 0;
}