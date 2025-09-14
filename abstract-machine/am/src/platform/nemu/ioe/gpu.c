#include <am.h>
#include <nemu.h>
#include <string.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

#define GPU_WIDTH 300
#define GPU_HEIGHT 400

void __am_gpu_init() {
  int i;
  int w = GPU_WIDTH;  // TODO: get the correct width
  int h = GPU_HEIGHT;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 0x0;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = GPU_WIDTH, .height = GPU_HEIGHT,
    .vmemsz = GPU_HEIGHT * GPU_WIDTH * 32
  };
}

// AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int fb_width = GPU_WIDTH;
  int fb_height = GPU_HEIGHT;

  // 边界检查（保持不变）
  if (ctl->x < 0 || ctl->y < 0 || 
      ctl->x + ctl->w > fb_width || 
      ctl->y + ctl->h > fb_height) {
    return;
  }

  uint32_t *pixels = (uint32_t *)ctl->pixels;
  if (pixels == NULL) return;

  // 计算帧缓冲中绘制区域的起始地址（按行优先存储）
  uint32_t *fb_base = (uint32_t *)FB_ADDR;
  // 每行像素在帧缓冲中的起始索引 = 起始行 * 屏幕宽度 + 起始列
  uint32_t fb_start_row = ctl->y;
  uint32_t fb_start_col = ctl->x;

  // 优化：按行批量复制，利用CPU缓存和连续内存访问
  for (int h = 0; h < ctl->h; h++) {
    // 计算当前行在帧缓冲中的起始指针
    uint32_t *fb_row_ptr = fb_base + (fb_start_row + h) * fb_width + fb_start_col;
    // 计算当前行在输入像素数组中的起始指针（按行优先）
    uint32_t *pix_row_ptr = pixels + h * ctl->w;

    // 批量复制一行像素（替代逐像素的outl调用）
    // 注意：这里假设FB_ADDR映射的内存可直接访问，无需通过outl逐字节写入
    memcpy(fb_row_ptr, pix_row_ptr, ctl->w * sizeof(uint32_t));
  }

  // 同步操作（保持不变）
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
