/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
#include "../monitor/sdb/sdb.h"

#ifdef CONFIG_MTRACE
  #define mringbuf_len 20
  static char * mringbuf[mringbuf_len];
  static int mringbuf_index = 0;
  static int mringbuf_full = 0;
#endif

void init_mringbuf(){
  #ifdef CONFIG_MTRACE
    for (int i = 0; i < mringbuf_len; i++){
      mringbuf[i] = (char *)malloc(128);
      memset(mringbuf[i], '\0', 128);
    }
  #endif
}

void printf_mringbuf(){
  #ifdef CONFIG_MTRACE
    printf("THE NEARING MEMORY OPERATIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (mringbuf_full){
      for (int i = mringbuf_index; i < mringbuf_len; i++){
        printf("       ");
        printf("%s\n", mringbuf[i]);
      }
    }

    for (int i = 0; i < mringbuf_index; i++){
      printf("       ");
      printf("%s\n", mringbuf[i]);
    }

    printf("-----------------------------------------------\n");
    printf("\n");
  #endif
}

word_t vaddr_ifetch(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  word_t read_data = paddr_read(addr, len);
  #ifdef CONFIG_MTRACE
    snprintf(mringbuf[mringbuf_index], 128, "read:  addr: %x  data: %d", addr, read_data);
    mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
    mringbuf_full = mringbuf_full || (mringbuf_index == 0);
  #endif
  return read_data;
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  paddr_write(addr, len, data);
  #ifdef CONFIG_MTRACE
    snprintf(mringbuf[mringbuf_index], 128, "write: addr: %x  data: %d", addr, data);
    mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
    mringbuf_full = mringbuf_full || (mringbuf_index == 0);
  #endif
}
