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

int printf(const char *fmt, ...) {
  // int cnt = 0;
  // const char * flag = fmt;
  // while(*fmt != '\0') {
  //   putch(*fmt);
  //   fmt++;
  //   cnt++;
  // }


  // return cnt;

  // fmt = flag;


  int count = 0;
  va_list ap;
  va_start(ap, fmt);

  while (*fmt != '\0'){
    if (*fmt != '%'){
      putch(*fmt++);
      count++;
      continue;
    }
    fmt++;
    switch(*fmt){
      case 'd':{
        int num = va_arg(ap, int);
        // char *buf = malloc(23* sizeof(char));
        
        char buf[32];
        memset(buf, '\0', 32);
        int_to_str(num, buf);
        int i = 0;
        while (buf[i] != '\0') {
          putch(buf[i]);
          i++;
          count++;
        }
        // free(buf);
        fmt++;
        break;
      }

      case 's' :{
        char *str = va_arg(ap, char *);
        while (*str != '\0') {
          putch(*str);
          str++;
          count++;
        }
        fmt++;
        break;
      }

      case '%' :{
        putch('%');
        count++;
        fmt++;
        break;
      }
      default: {
        putch(*fmt);
        count++;
        fmt++;
        break;
      }
    }
  }

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

  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

