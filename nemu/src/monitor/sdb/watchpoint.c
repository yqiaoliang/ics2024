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

#include "sdb.h"
// #include "../monitor.h"
#include <assert.h>

#define NR_WP 32

// typedef struct watchpoint {
//   int NO;
//   struct watchpoint *next;
//   char *expr;

//   /* TODO: Add more members if necessary */

// } WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

WP * get_wp_list() {
  return head;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP * new_wp(char *expr_) {
  if (free_ == NULL) {
    printf("No more watchpoints available.\n");
    return NULL;
  }

  WP *wp = free_;
  free_ = free_->next;

  wp->next = head;
  wp->expr = strdup(expr_); // Duplicate the expression string
  bool result = true;
  bool *success = &result;
  wp->expr_result = expr(wp->expr, success);
  if (wp->expr == NULL || !result) {
    printf("Failed to allocate memory for watchpoint expression.\n");
    free(wp);
    return NULL;
  }

  head = wp;
  printf("success to create watchpoint %d\n", wp->NO);

  return wp;
}

void free_wp(int NO_) {

  // Remove from the linked list
  if (NO_ == head->NO) {
    head = head->next;
  } 
  else {
    WP *prev = head;
    while (prev != NULL && prev->next->NO != NO_) {
      prev = prev->next;
    }
    // if (prev != NULL) {
    //   prev->next = wp->next;
    // }
    if (prev == NULL) {
      printf("there is not exist %d watchpoint ", NO_);
    }

    WP *cur = prev->next;
    prev->next = cur->next;
    cur->next = free_;
    free_ = cur;

  }

  // Add back to the free list
  
  // wp->next = free_;
  printf("free NO %d watchpoint success" ,NO_);
  // free_ = wp;
}

void print_all_wp(){
  WP *pre = head;

  while(pre != NULL){
    printf("NO. %d, expr: %s \n" ,pre->NO, pre->expr);
    pre = pre->next;
  }

}