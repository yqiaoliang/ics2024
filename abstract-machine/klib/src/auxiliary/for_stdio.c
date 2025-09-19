#include "for_stdio.h"

char * int_to_str(int num, char *buf) {
  int is_negative = 0;
  int i = 0;
  unsigned int n;
  char temp[64]; 

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


int int_to_str_for_printf(int num, int width) {
  int is_negative = 0;
  int i = 0;
  unsigned int n = 0;
  char temp[64]; 
  int count = 0;

  if (num == 0) {
    putch('0');
    count++;
    for (int j = 1; j < width; j++){
      putch('0');
      count++;
    }
    return count;
  }

  if (num < 0) {
    is_negative = 1;
    n = (unsigned int)(num < 0 ? -num : num);
  } 
  else {
    n = (unsigned int)num;
  }

  while (n != 0) {
    temp[i] = '0' + (n % 10);
    i++;
    n /= 10;
  }

  if (is_negative){
    putch('-');
    count++;
  }

  for (int j = count + i; j < width; j++){
    putch('0');
    count++;
  }

  while (i > 0) {
    putch(temp[i-1]);
    i--;
    count++;
  }

  return count;
}

