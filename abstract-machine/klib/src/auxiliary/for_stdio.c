#include "for_stdio.h"

char * int_to_str(int num, char *buf) {
  int is_negative = 0;
  int i = 0;
  unsigned int n;
  char temp[32]; 

  if (num == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return buf+1;
  }

  if (num < 0) {
    is_negative = 1;
    n = (unsigned int)(-(long long)num);
  } else {
    n = (unsigned int)num;
  }

  while (n != 0) {
    temp[i++] = '0' + (n % 10);
    n /= 10;
  }


  if (is_negative) {
    *buf++ = '-';
  }

  while (i > 0) {
    *buf++ = temp[--i];
  }
  *buf = '\0'; 

  return buf;
}


int vsprintf_helper(char *out, const char *fmt, va_list ap) {
    char *start = out;  // 保存起始地址用于计算长度

    while (*fmt) {
        if (*fmt != '%') {
            *out++ = *fmt++;
            continue;
        }
        fmt++;
        switch (*fmt) {
            case 'd': {
                int num = va_arg(ap, int);
                out = int_to_str(num, out);
                fmt++;
                break;
            }
            case 's': {
                char *str = va_arg(ap, char *);
                while (*str) {
                    *out++ = *str++;
                }
                fmt++;
                break;
            }
            case '%': {
                *out++ = '%';
                fmt++;
                break;
            }
            default:
                *out++ = '%';
                if (*fmt) fmt++;
                break;
        }
    }

    *out = '\0';  // 添加终止符
    return out - start;  // 返回写入的字符数（不包括终止符）
}