#include "monitor.h"
#include "stdio.h"
#include <stdbool.h>

int main(){
    char * this_expr = "2 + 3 * 5 -(8 * 9)";
    bool *success = true;
    printf(expr(this_expr, success));
}