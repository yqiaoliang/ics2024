#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include <stdio.h>

static Dtrace * dtrace = NULL;

void init_dringbuf(){
    dtrace = (Dtrace *) malloc(sizeof(Dtrace));
    dtrace->dringbuf_len = DRINGBUF_LEN;
    dtrace->dringbuf_index = 0;
    dtrace->dringbuf_full = 0;
    for (int i = 0; i < DRINGBUF_LEN; i++){
      dtrace->dringbuf[i] = (char *)malloc(128);
      memset(dtrace->dringbuf[i], '\0', 128);
    }
}

void printf_dringbuf(){
    printf("ERROR HAPPEND, THE NEARING INSTRUCTIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (dtrace->dringbuf_full){
      for (int i = dtrace->dringbuf_index; i < dtrace->dringbuf_len; i++){
        printf("       ");
        printf("%s\n", dtrace->dringbuf[i]);
      }
    }

    for (int i = 0; i < dtrace->dringbuf_index; i++){
      if (i == dtrace->dringbuf_index - 1) printf("ERROR: ");
      else printf("       ");
      printf("%s\n", dtrace->dringbuf[i]);
    }

    printf("-----------------------------------------------\n");
    printf("\n");
}

Dtrace * get_dtrace(){
  return dtrace;
}
