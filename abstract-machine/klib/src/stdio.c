#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include "./auxiliary/for_stdio.h"


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  int count = 0;
  va_list ap;
  va_start(ap, fmt);
  
  while (*fmt) {
      if (*fmt != '%') {
          putch(*fmt);
          count++;
          fmt++;
          continue;
      }
      
      fmt++; // 跳过 '%'
      if (*fmt == '\0') break;
      
      switch (*fmt) {
          case 'd': {
              int num = va_arg(ap, int);
              char buf[32];
              int_to_str(num, buf);
              for (char *p = buf; *p; p++) {
                  putch(*p);
                  count++;
              }
              fmt++;
              break;
          }
          case 'x': case 'X': {
              unsigned int num = va_arg(ap, unsigned int);
              char hex_digits[] = "0123456789abcdef";
              if (*fmt == 'X') {
                  hex_digits[10] = 'A'; // 大写字母
                  hex_digits[11] = 'B';
                  hex_digits[12] = 'C';
                  hex_digits[13] = 'D';
                  hex_digits[14] = 'E';
                  hex_digits[15] = 'F';
              }
              
              char temp[9];
              int i = 0;
              
              // 处理0值
              if (num == 0) {
                  putch('0');
                  count++;
              } else {
                  while (num != 0) {
                      temp[i++] = hex_digits[num & 0xF];
                      num >>= 4;
                  }
                  
                  // 反转字符串
                  while (i > 0) {
                      putch(temp[--i]);
                      count++;
                  }
              }
              fmt++;
              break;
          }
          case 's': {
              char *str = va_arg(ap, char *);
              while (*str) {
                  putch(*str);
                  str++;
                  count++;
              }
              fmt++;
              break;
          }
          case '%': {
              putch('%');
              count++;
              fmt++;
              break;
          }
          default: {
              putch('%');
              putch(*fmt);
              count += 2;
              fmt++;
              break;
          }
      }
  }
  
  va_end(ap);
  return count;
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

  va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

