#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
char * int_to_str(int num, char *buf) {
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

static void print_str(const char *str) {
    while (*str) {
        putch(*str++);
    }
}

// 辅助函数：输出整数
static void print_int(int num) {
    if (num < 0) {
        putch('-');
        num = -num;
    }
    
    // 处理0的特殊情况
    if (num == 0) {
        putch('0');
        return;
    }
    
    // 将数字转换为字符（逆序）
    char buf[20];
    int i = 0;
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // 逆序输出
    while (i > 0) {
        putch(buf[--i]);
    }
}

// 自定义printf实现
int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    int count = 0;  // 统计输出的字符数
    
    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;  // 跳过'%'
            switch (fmt[i]) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putch(c);
                    count++;
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char*);
                    if (str == NULL) {
                        print_str("(null)");
                        count += 6;
                    } else {
                        int len = strlen(str);
                        print_str(str);
                        count += len;
                    }
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    // 计算数字的字符长度
                    int temp = num;
                    int len = 0;
                    if (temp < 0) {
                        len++;  // 负号
                        temp = -temp;
                    }
                    if (temp == 0) {
                        len = 1;
                    } else {
                        while (temp > 0) {
                            len++;
                            temp /= 10;
                        }
                    }
                    print_int(num);
                    count += len;
                    break;
                }
                case '%': {
                    putch('%');
                    count++;
                    break;
                }
                default: {
                    // 处理未知格式符，直接输出
                    putch('%');
                    putch(fmt[i]);
                    count += 2;
                    break;
                }
            }
        } else {
            putch(fmt[i]);
            count++;
        }
    }
    
    va_end(args);
    return count;  // 返回输出的字符总数
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  while(*fmt) {
    if (*fmt != '%') {
      *out++ = *fmt++;
      continue;
    }
    fmt++;
    switch(*fmt) {
      case 'd':{
        int num = va_arg(ap, int);
        out = int_to_str(num, out);
        fmt++;
        break;
      }
      case 's':{
        char *str = va_arg(ap, char *);
        while(*str){
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
      default: fmt++; break;
    }
  }

  while(*out) *out++ = '\0';

  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

