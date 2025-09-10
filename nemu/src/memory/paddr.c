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

#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>

#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_GARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif

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
      memset(mringbuf[i], 0, 128);
    }
  #endif
}

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
  IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

word_t paddr_read(paddr_t addr, int len) {
  if (likely(in_pmem(addr))) {
    word_t read_data = pmem_read(addr, len);
    #ifdef CONFIG_MTRACE
      snprintf(mringbuf[mringbuf_index], 128, "read: addr: %x  data: %d", addr, read_data);
      mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
      mringbuf_full = mringbuf_full || (mringbuf_index == 0);
    #endif

    return read_data;
  }
  IFDEF(CONFIG_DEVICE, 
    word_t read_data = mmio_read(addr, len);
    #ifdef CONFIG_MTRACE
      snprintf(mringbuf[mringbuf_index], 128, "mmio read: addr: %x  data: %d", addr, read_data);
      mringbuf_index = (mringbuf_index + 1) % mringbuf_len;
      mringbuf_full = mringbuf_full || (mringbuf_index == 0)
    #endif
    return read_data
  );
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  out_of_bound(addr);
}
