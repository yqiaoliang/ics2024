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

  // while (*fmt) {
  //   putch(*fmt);
  //   fmt++;
  //   count++;
  // }
  // return count;
  
  while (*fmt){
    if (*fmt != '%'){
      putch(*fmt);
      fmt++;
      count++;
      continue;
    }
    fmt++;

    switch(*fmt){
      case 'd' : {
        putch('\n');
        count+=1;
        return count ;
        int num = va_arg(ap, int);
        count += int_to_str_for_printf(num);
        putch('\n');
        count+=1;
        return count;
        
        fmt++;
        break;
      }

      case 's' : {
        char * str = va_arg(ap, char *);
        while (*str){
          putch(*str);
          str++;
          count ++;
        }
        fmt++;
        break;
      }

      default:{
        putch('%');
        putch(*fmt);
        fmt++;
        count += 2;
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

