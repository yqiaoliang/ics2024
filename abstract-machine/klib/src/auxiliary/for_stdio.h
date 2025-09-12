#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

char * int_to_str(int num, char *buf);
int vsprintf_helper(char *out, const char *fmt, va_list ap);