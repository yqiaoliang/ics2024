// // #include "../sdb/sdb.c"
// #include "trace.h"

// #ifdef CONFIG_MTRACE
//   #define mringbuf_len 20
//   static char * mringbuf[mringbuf_len];
//   static int mringbuf_index = 0;
//   static int mringbuf_full = 0;
// #endif

// void init_mringbuf(){
//   #ifdef CONFIG_MTRACE
//     for (int i = 0; i < mringbuf_len; i++){
//       mringbuf[i] = (char *)malloc(128);
//       memset(mringbuf[i], '\0', 128);
//     }
//   #endif
// }

// void printf_mringbuf(){
//   #ifdef CONFIG_MTRACE
//     printf("THE NEARING MEMORY OPERATIONS ARE:\n");
//     printf("-----------------------------------------------\n");
//     if (mringbuf_full){
//       for (int i = mringbuf_index; i < mringbuf_len; i++){
//         printf("       ");
//         printf("%s\n", mringbuf[i]);
//       }
//     }

//     for (int i = 0; i < mringbuf_index; i++){
//       printf("       ");
//       printf("%s\n", mringbuf[i]);
//     }

//     printf("-----------------------------------------------\n");
//     printf("\n");
//   #endif
// }

// char * get_mringbuf(){
//     return mringbuf[mringbuf_index];
// }

// void update_mringbuf_idex(int idx, int is_full){
//     mringbuf_index = idx;
//     mringbuf_full = is_full;
// }


// int get_mringbuf_len() {
//     return mringbuf_len;
// }