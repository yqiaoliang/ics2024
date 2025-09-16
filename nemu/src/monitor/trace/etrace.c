#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <stdio.h>

static Etrace * etrace = NULL;

void init_eringbuf(){
    etrace = (Etrace *) malloc(sizeof(Etrace));
    etrace->eringbuf_len = ERINGBUF_LEN;
    etrace->eringbuf_index = 0;
    etrace->eringbuf_full = 0;
    for (int i = 0; i < ERINGBUF_LEN; i++){
      etrace->eringbuf[i] = (char *)malloc(128);
      memset(etrace->eringbuf[i], '\0', 128);
    }
}

void printf_eringbuf(){
    printf("ERROR HAPPEND, THE NEARING EXECUTION OPERATIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (etrace->eringbuf_full){
      for (int i = etrace->eringbuf_index; i < etrace->eringbuf_len; i++){
        printf("       ");
        printf("%s\n", etrace->eringbuf[i]);
      }
    }

    for (int i = 0; i < etrace->eringbuf_index; i++){
      printf("       ");
      printf("%s\n", etrace->eringbuf[i]);
    }

    printf("-----------------------------------------------\n");
    printf("\n");
}

Etrace * get_etrace(){
  return etrace;
}