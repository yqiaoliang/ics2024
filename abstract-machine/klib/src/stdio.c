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
  panic("Not implemented");
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
      default: break;
    }
  }

  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

// int main() {
//   char * buf = (char *)malloc(20);
//   int_to_str(123456, buf);
//   while (*buf){
//     printf("%c", *buf++);
//   }
// }
