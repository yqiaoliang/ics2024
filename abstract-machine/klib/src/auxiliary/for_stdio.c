#include "for_stdio.h"


#define INT_MAX 0x7fffffff
#define INT_MIN -0x80000000

char *int_to_str(int num, char *buf) {
    int is_negative = 0;
    int i = 0;
    unsigned int n;
    char temp[22]; 

    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    if (num < 0) {
        is_negative = 1;
        // 正确处理INT_MIN边界情况
        n = (num == INT_MIN) ? (unsigned int)INT_MAX + 1 : (unsigned int)(-num);
    } else {
        n = (unsigned int)num;
    }

    while (n != 0) {
        temp[i++] = '0' + (n % 10);
        n /= 10;
    }

    char *start = buf;
    if (is_negative) {
        *buf++ = '-';
    }

    while (i > 0) {
        *buf++ = temp[--i];
    }
    *buf = '\0';

    return start;
}