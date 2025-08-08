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

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

// word_t expr(char *e, bool *success);
word_t vaddr_read(vaddr_t addr, int len);
int expr(char *e, bool *success);

typedef struct watchpoint WP;
WP *new_wp();
int expr(char *e, bool *success);
WP *get_wp_list();
char * get_rand_expr();
// bool is_parentheses_match(int p, int q);

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *expr;
  int expr_result;

  /* TODO: Add more members if necessary */

} WP;


#endif
