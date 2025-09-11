#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include <stdio.h>

static Itrace * itrace = NULL;

void init_iringbuf(){
    itrace = (Itrace *) malloc(sizeof(Itrace));
    itrace->iringbuf_len = IRINGBUF_LEN;
    itrace->iringbuf_index = 0;
    itrace->iringbuf_full = 0;
    for (int i = 0; i < IRINGBUF_LEN; i++){
      itrace->iringbuf[i] = (char *)malloc(128);
      memset(itrace->iringbuf[i], '\0', 128);
    }
}

void printf_iringbuf(){
    printf("ERROR HAPPEND, THE NEARING INSTRUCTIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (itrace->iringbuf_full){
      for (int i = itrace->iringbuf_index; i < itrace->iringbuf_len; i++){
        // if (i == iringbuf_len - 1) printf("ERROR: ");
        printf("       ");
        printf("%s\n", itrace->iringbuf[i]);
      }
    }

    for (int i = 0; i < itrace->iringbuf_index; i++){
      if (i == itrace->iringbuf_index - 1) printf("ERROR: ");
      else printf("       ");
      printf("%s\n", itrace->iringbuf[i]);
    }

    printf("-----------------------------------------------\n");
    printf("\n");
}

Itrace * get_itrace(){
  return itrace;
}