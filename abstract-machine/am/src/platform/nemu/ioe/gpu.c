#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

#define GPU_WIDTH 400
#define GPU_HEIGHT 300

void __am_gpu_init() {
  int i;
  int w = 400;  // TODO: get the correct width
  int h = 300;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = GPU_WIDTH, .height = GPU_HEIGHT,
    .vmemsz = 0
  };
}

// AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // 1. 获取帧缓冲配置（宽度）
  int fb_width = 400;  // 实际应从config中获取，如通过全局变量或函数
  int fb_height = 300;

  // 2. 边界检查：确保绘制区域在帧缓冲内
  if (ctl->x < 0 || ctl->y < 0 || 
      ctl->x + ctl->w > fb_width || 
      ctl->y + ctl->h > fb_height) {
    return;  // 越界则不绘制
  }

  // 3. 转换像素指针（假设32位像素，与帧缓冲格式一致）
  uint32_t *pixels = (uint32_t *)ctl->pixels;
  if (pixels == NULL) return;

  // 4. 逐行逐列写入帧缓冲
  for (int row = 0; row < ctl->h; row++) {
    for (int col = 0; col < ctl->w; col++) {
      // 计算当前像素在帧缓冲中的索引
      int fb_x = ctl->x + col;
      int fb_y = ctl->y + row;
      int fb_index = fb_y * fb_width + fb_x;

      // 计算内存地址（每个像素4字节）
      uint32_t addr = FB_ADDR + fb_index * 4;

      // 写入像素数据
      outl(addr, pixels[row * ctl->w + col]);
    }
  }

  // 同步（如果需要）
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
