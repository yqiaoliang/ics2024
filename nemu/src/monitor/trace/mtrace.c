#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include <stdio.h>

static Mtrace * mtrace = NULL;

void init_mringbuf(){
    mtrace = (Mtrace *) malloc(sizeof(Mtrace));
    mtrace->mringbuf_len = MRINGBUF_LEN;
    mtrace->mringbuf_index = 0;
    mtrace->mringbuf_full = 0;
    for (int i = 0; i < MRINGBUF_LEN; i++){
      mtrace->mringbuf[i] = (char *)malloc(128);
      memset(mtrace->mringbuf[i], '\0', 128);
    }
}

void printf_mringbuf(){
    printf("ERROR HAPPEND, THE NEARING MEMORY OPERATIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (mtrace->mringbuf_full){
      for (int i = mtrace->mringbuf_index; i < mtrace->mringbuf_len; i++){
        printf("       ");
        printf("%s\n", mtrace->mringbuf[i]);
      }
    }

    for (int i = 0; i < mtrace->mringbuf_index; i++){
      printf("       ");
      printf("%s\n", mtrace->mringbuf[i]);
    }

    printf("-----------------------------------------------\n");
    printf("\n");
}

Mtrace * get_mtrace(){
  return mtrace;
}