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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
// #include "./gen-expr.h"
#include "./sdb/sdb.h"


// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_num(){
  int num = rand() % 100;
  num += 1;
  char str_num[8];
  sprintf(str_num, "%d", num);
  strcat(buf, str_num);
}

static void gen_left(){
  char left_char[2] = {'(', '\0'};
  strcat(buf, left_char);
}

static void gen_right(){
  char right_char[2] = {')', '\0'};
  strcat(buf, right_char);
}

static void gen_operate(){
  int num = rand() % 4;
  char operate_char[2];
  operate_char[1] = '\0';
  switch(num){
    case 0 :
      operate_char[0] = '+';
      strcat(buf, operate_char);
      return;
    case 1 :
      operate_char[0] = '-';
      strcat(buf, operate_char);
      return;
    case 2 :
      operate_char[0] = '*';
      strcat(buf, operate_char);
      return;
    case 3 :
      operate_char[0] = '/';
      strcat(buf, operate_char);
      return;
    default :
      operate_char[0] = '+';
      strcat(buf, operate_char);
      return;
  }
}


static void gen_rand_expr(int func_deep) {
  if (func_deep == 0){
    buf[0] = '\0';
  }

  if (func_deep > 5) {
    gen_num();
    return;
  }

  int choose = rand() % 3;
  if (choose == 0) gen_num();
  else if (choose == 1) {
    gen_left();
    gen_rand_expr(func_deep + 1);
    gen_right();
  }
  else  {
    gen_rand_expr(func_deep + 1);
    gen_operate();
    gen_rand_expr(func_deep + 1);
  }
  
}

char * get_rand_expr(){
  // int seed = time(0);
  // srand(seed);
  gen_rand_expr(0);
  // printf("buf: %s\n", buf);

  return  buf;

}


int main_test(int argc, int loop) {
  int seed = time(0);
  srand(seed);

  int i;
  int error_num = 0;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr(0);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    bool success = true;
    bool * success_ptr = &success;
    long my_expr_reslut = expr(buf, success_ptr);
    printf("expr: %s\n", buf);
    printf("my_reslut: %u\n", result);
    printf("result: %u\n", result);
    if (my_expr_reslut == result) printf("correct\n");
    else {
      printf("error\n");
      error_num += 1;
    }
    printf("\n");




    // printf("my_reslut: %u %u %s\n", result, buf);
  }

  printf("error_num: %d", error_num);
  return 0;
}
