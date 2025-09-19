#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s++) len++;
  return len;
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
  while(*src) {
    *dst++ = *src++;
  }
  *dst = '\0';
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char * ret = dst;
  while(n && *src) {
    *dst++ = *src++;
    n--;
  }
  while(n--) {
    *dst++ = '\0';
  }
  return ret;
}

char *strcat(char *dst, const char *src) {
  char *ret = dst;
  while(*dst) dst++;
  while(*src) {
    *dst++ = *src++;
  }
  *dst = '\0';
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }
  return n ? *(unsigned char *)s1 - *(unsigned char *)s2 : 0;
}

void *memset(void *s, int c, size_t n) {
  char *ptr = s;
  while (n--) {
    *ptr++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = dst;
  const char *s = src;
  if (d < s) {
    while (n--) {
      *d++ = *s++;
    }
  } else {
    d += n;
    s += n;
    while (n--) {
      *(--d) = *(--s);
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = out;
  const char *s = in;
  while (n--) {
    *d++ = *s++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1;
  const unsigned char *p2 = s2;
  while (n--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

#endif
