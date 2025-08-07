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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
  tk_notype = 256, tk_eq tk_add, tk_sub, tk_mul, tk_div, tk_left, tk_right, tk_num, tk_bool_eq,
  tk_bool_uneq, tk_add, tk_ptr

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", tk_notype},    // spaces
  {"\\+", tk_add},         // plus
  {"\\-", tk_sub},
  {"\\*", tk_mul},         
  {"\\/", tk_div},
  {"\\(", tk_left},         
  {"\\)", tk_right},
  {"[0-9]+", tk_num}, 
  {"==", tk_bool_eq},
  {"!=", tk_bool_uneq},
  {"&&", tk_and},
  {"==", tk_eq},        // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case tk_notype : break;
          default: 
            if (nr_token >= ARRLEN(tokens)) {
              Log("To many tokens in expression");
              return false;
            }
            if(substr_len > 31) Log("at least one of token to long, over 31 chars");
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  if (!is_parentheses_match(0, nr_token-1)){
    *success = false;
    return 0;
  }
  return eval(0, nr_token-1, success);
}


bool is_parentheses_match(int p, int q){
  int match = 0;

  for (int i = p; i <= q; i++){
    if (tokens[i].type == tk_left) match += 1;
    else if (token[i].type == tk_right) mathc -= 1;
  }

  return match == 0
}

int find_operate_pos(int p, int q){
  for (int i = q; i <= p; i--){
    if (tokens[i].type == tk_add || tokens[i].type == tk_sub) reutrn i;
  }

  for (int i = p; i <= q; i++){
    if (tokens[i] == tk_mul || tokens[i].type == tk_div) return i;
  }

  return -1;
}


word_t eval(int p, int q, bool *success){
  if (! *success) return 0;

  if (p > q){
    printf("ERROR occur in the expression : p > q");
    *success = false;
    return 0;
  }

  else if (p == q){
    return atoi(tokens[p].str);
  }

  else if (tokens[p].type == tk_left && tokens[q].type == tk_right) {
    return eval(p + 1, q - 1, success);
  }

  else {
    int operate_pos = find_operate_pos(p, q);
    word_t eval_left = eval(p, operate_pos-1, success);
    word_t eval_right = evla(operate_pos+1, q, success); 

    switch(token[operate_pos].type){
      case tk_add : return eval_left + eval_right;
      case tk_sub : return eval_left - eval_right;
      case tk_mul : return eval_left * eval_right;
      case tk_div :
          if (eval_right == 0) {
            printf("ERROR the expression exist div 0")
            *success = false;
            return 0;
          }
          return eval_left / eval_right;
    }
  }
}

int main(){
    char * this_expr = "2 + 3 * 5 -(8 * 9)";
    bool *success = true;
    printf(expr(this_expr, success));
}