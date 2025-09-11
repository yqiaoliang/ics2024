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

word_t vaddr_ifetch(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  word_t read_data = paddr_read(addr, len);
  #ifdef CONFIG_MTRACE
    // snprintf(get_mringbuf(), 128, "read:  addr: %0x  data: %d", addr, read_data);
    // // mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
    // // mringbuf_full = mringbuf_full || (mringbuf_index == 0);
    // update_mringbuf_idex((mringbuf_index + 1) % get_mringbuf_len(), mringbuf_full || (mringbuf_index == 0))
  #endif
  return read_data;
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  paddr_write(addr, len, data);
  #ifdef CONFIG_MTRACE
    // snprintf(get_mringbuf(), 128, "write: addr: %0x  data: %d", addr, data);
    // // mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
    // // mringbuf_full = mringbuf_full || (mringbuf_index == 0);
    // update_mringbuf_idex((mringbuf_index + 1) % get_mringbuf_len(), mringbuf_full || (mringbuf_index == 0))
  #endif
}
