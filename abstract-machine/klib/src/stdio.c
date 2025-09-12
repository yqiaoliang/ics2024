#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include "./auxiliary/for_stdio.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
      va_list ap;
    char buffer[1024];  // 用于存储格式化后的字符串
    int len;

    // 1. 使用va_start初始化参数列表
    va_start(ap, fmt);
    
    // 2. 利用已有的sprintf将格式化结果写入缓冲区
    // 这里需要一个支持可变参数的中间函数
    len = vsprintf(buffer, fmt, ap);
    
    // 3. 结束参数列表处理
    va_end(ap);
    
    // 4. 将缓冲区内容写入标准输出（文件描述符1）
    char *p = buffer;
    while(*p++)putch(*p);
    
    return len;  // 返回输出的字符数
}

int vsprintf(char *out, const char *fmt, va_list ap) {
      va_list ap_copy;
    va_copy(ap_copy, ap);
    
    int len = vsprintf_helper(out, fmt, ap_copy);
    
    va_end(ap_copy);
    return len;
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

