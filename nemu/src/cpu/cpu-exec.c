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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
// #include "../monitor/monitor.h"
#include "../monitor/sdb/sdb.h"

#ifdef CONFIG_ITRACE
  #define iringbuf_len 15
  static char * iringbuf[iringbuf_len];
  static int iringbuf_index = 0;
  static int iringbuf_full = 0;
#endif

void init_iringbuf(){
  #ifdef CONFIG_ITRACE
    for (int i = 0; i < iringbuf_len; i++){
      iringbuf[i] = (char *)malloc(128);
      memset(iringbuf[i], 0, 128);
    }
  #endif
}

void printf_iringbuf(){
  #ifdef CONFIG_ITRACE
    printf("ERROR HAPPEND, THE NEARING INSTRUCTIONS ARE:\n");
    printf("-----------------------------------------------\n");
    if (iringbuf_full){
      for (int i = iringbuf_index; i < iringbuf_len; i++){
        // if (i == iringbuf_len - 1) printf("ERROR: ");
        printf("       ");
        printf("%s\n", iringbuf[i]);
      }
    }

    for (int i = 0; i < iringbuf_index; i++){
      if (i == iringbuf_index - 1) printf("ERROR: ");
      else printf("       ");
      printf("%s\n", iringbuf[i]);
    }

    printf("-----------------------------------------------\n");
  #endif
}

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  WP *head = get_wp_list();

  char * nemu_state_names[] = {"NEMU_RUNNING", "NEMU_STOP", "NEMU_END", "NEMU_ABORT", "NEMU_QUIT"};
  while(head != NULL){
    bool success = true;
    int result = expr(head->expr, &success);
    if (result != head->expr_result) {
      printf("pre nemu state: %s \n", nemu_state_names[nemu_state.state]);
      if (nemu_state.state == NEMU_RUNNING) nemu_state.state = NEMU_STOP;
      printf("The system triggered a monitoring point, current nemu state: %s \n", nemu_state_names[nemu_state.state]);
      return;
    }
    head = head->next;
  }
  
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
  printf("test0\n");
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  char *iringbuf_p = iringbuf[iringbuf_index];
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  iringbuf_p += snprintf(iringbuf_p, 128, FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst;
#ifdef CONFIG_ISA_x86
  for (i = 0; i < ilen; i ++) {
#else
  for (i = ilen - 1; i >= 0; i --) {
#endif
    p += snprintf(p, 4, " %02x", inst[i]);
    iringbuf_p += snprintf(iringbuf_p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  memset(iringbuf_p, ' ', space_len);
  p += space_len;
  iringbuf_p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst, ilen);
  disassemble(iringbuf_p, 128 - (iringbuf_p - iringbuf[iringbuf_index]),
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst, ilen);

  iringbuf_index = (iringbuf_index + 1) % iringbuf_len;
  iringbuf_full = iringbuf_full || (iringbuf_index == 0);
#endif
}


static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  #ifdef CONFIG_ITRACE
    printf("CONFIG_ITRACE\n");
  #endif
  #ifdef CONFIG_MTRACE
    printf("CONFIG_MTRACE\n");
  #endif

  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT: case NEMU_QUIT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();
  if (n < 0 ){
    while (nemu_state.state == NEMU_RUNNING) {
      execute(1);
    }
  }
  else execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:{
      #ifdef CONFIG_ITRACE
        if (nemu_state.state == NEMU_ABORT) printf_iringbuf();
        else if (nemu_state.halt_ret != 0) printf_iringbuf();
      #endif

      #ifdef CONFIG_MTRACE
        if (nemu_state.state == NEMU_ABORT) printf_mringbuf();
        else if (nemu_state.halt_ret != 0) printf_mringbuf();
      #endif

      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      }
      // fall through
    case NEMU_QUIT: statistic();
  }
}
