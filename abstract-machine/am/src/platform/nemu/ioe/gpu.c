#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

#define GPU_WIDTH 200
#define GPU_HEIGHT 100

void __am_gpu_init() {
  int i;
  int w = GPU_WIDTH;  // TODO: get the correct width
  int h = GPU_HEIGHT;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 0;
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

  if (ctl->x < 0 || ctl->y < 0 || 
      ctl->x + ctl->w > fb_width || 
      ctl->y + ctl->h > fb_height) {
    return;
  }

  uint32_t *pixels = (uint32_t *)ctl->pixels;
  if (pixels == NULL) return;

  // for (int row = 0; row < ctl->h; row++) {
  //   for (int col = 0; col < ctl->w; col++) {
  //     int fb_x = ctl->x + col;
  //     int fb_y = ctl->y + row;
  //     int fb_index = fb_x  + fb_y;

  //     uint32_t addr = FB_ADDR + fb_index * 4;

  //     outl(addr, pixels[row * ctl->w + col]);
  //   }
  // }

  for (int w = 0; w < ctl->w; w++){
    for (int h = 0; h < ctl->h; h++){
      int fb_x = ctl->x + w;
      int fb_y = ctl->y + h;

      int fb_index = fb_y + fb_x;

      uint32_t addr = FB_ADDR + fb_index * 4;
      outl(addr, pixels[w * ctl->h + h]);
    }
  }

  // for (int w = 0; w < ctl->w; w++){
  //   for (int h = 0; h < ctl->h; h++){
  //     int fb_x = ctl->x + h;
  //     int fb_y = ctl->y + w;

  //     int fb_index = fb_y * fb_height + fb_x;

  //     uint32_t addr = FB_ADDR + fb_index * 4;
  //     outl(addr, pixels[w * ctl->h + h]);
  //   }
  // }



  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
