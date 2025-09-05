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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  if (args == NULL) {
    cpu_exec(1);
    return 0;
  }
  for (int i = 0; i < atoi(args); i++) {
    cpu_exec(1);
  }
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
    printf("execute 1 step\n");
    return 0;
  }

  printf("execute %s  step\n", args);
  cpu_exec(atoi(args));
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL || (strcmp(args, "r") == 0)) {
    isa_reg_display();
    return 0;
  }

  if ((strcmp(args, "w") == 0)) print_alive_wp();
  
  return 0;
}

static int cmd_p(char *args){
  bool result = true;
  bool *success = &result;
  uint32_t expr_reslut = expr(args, success);
  if (result) printf("result: %u \n", expr_reslut);

  return 0;
}

static int cmd_w(char *args){
  new_wp(args);
  return 0;
}

static int cmd_print_alive_wp(char * args){
  print_alive_wp();
  return 0;
}

static int cmd_d (char * args){
  int NO_ = atoi(args);
  free_wp(NO_);
  return 0;
}

static int cmd_print_free_wp(char * args){
  print_free_wp();
  return 0;
}

static int cmd_x(char *args){
  printf("args: %s\n", args);
  if (args == NULL) {
    printf("please enter addr\n");
    return 0;
  }

  char *num_str = strtok(args, " ");
  int num = atoi(num_str);
  if (num <= 0) return 0;

  char *expr_str = strtok(NULL, "");
  printf("expr_str: %s\n", expr_str);
  char *expr_ = NULL; 
  expr_ = malloc(strlen(expr_str) + 1);
  strcpy(expr_, expr_str);
  bool success = true;
  bool * success_ptr = &success;
  uint32_t test = expr(expr_, success_ptr);
  if (success_ptr){
    for (int i = 0; i < num; i +=4){
      printf("%0x \n", vaddr_read(test+i, 4));
    }
  }
  else printf("your expression is wrong");


  // if (num <= 0 || expr_ == NULL) return 0;
  // bool result  = true;
  // bool * success = &result;
  // int addr_ = expr(expr_, success);
  // if (! result) return 0;
  // for (int i = 0; i < num; i++){
  //   vaddr_read(addr_, 4);
  // } 

  return 0;

}



static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute n times instruction and stop", cmd_si },
  { "info", "Display information about the program state", cmd_info },
  {"x", "Examine memory at a given address", cmd_x },
  {"p", "evalutation expression that you gave", cmd_p},
  {"w", "generate a new watchpoint", cmd_w},
  {"alive_wp", "print all watchpoint that alive", cmd_print_alive_wp},
  {"free_wp", "print all watchpoint that free", cmd_print_free_wp},
  {"d", "delete watchpoint that alive", cmd_d},


  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
