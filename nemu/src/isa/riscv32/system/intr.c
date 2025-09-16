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
#include <trace.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  cpu.csr.mepc = epc;
  cpu.csr.mcause = NO;

  Etrace * etrace = get_etrace();
  snprintf(etrace->eringbuf[etrace->eringbuf_index], 128, "trap addr: 0x%0x,  current addr: 0x%0x", cpu.csr.mtvec, epc);
  etrace->eringbuf_index = (etrace->eringbuf_index + 1) % etrace->eringbuf_len;
  etrace->eringbuf_full = etrace->eringbuf_full || (etrace->eringbuf_index == 0);


  return cpu.csr.mtvec;
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
