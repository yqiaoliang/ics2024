#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include "./auxiliary/for_stdio.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  int count = 0;
  va_list ap;

  char all_num[] = "0123456789";
  const char * cur = fmt;
  int is_format = 0;
  while (*cur != '\0'){
    if (*cur== '%')  {
      va_start(ap, fmt);
      is_format = 1;
      break;
    }
    cur++;
  }




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
        case 'd': case 'x': {
            int num = va_arg(ap, int);
            while(num){
              putch(all_num[num%10]);
              num /= 10;
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
            // 输出无效格式说明符（如 %x -> 输出 %x）
            putch('%');
            putch(*fmt);
            count += 2;
            fmt++;
            break;
        }
    }
        
  }


  if (is_format) va_end(ap);
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

