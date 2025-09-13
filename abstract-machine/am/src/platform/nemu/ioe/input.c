#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  static bool is_press = false;
  uint8_t kbd_data = inb(KBD_ADDR);


  if (kbd_data == AM_KEY_NONE) {
    kbd->keydown = 0;
    kbd->keycode = AM_KEY_NONE;
    is_press = false;
    return;
  }
  else {
    if (is_press){
      kbd->keydown = 0;
      kbd->keycode = kbd_data;
      is_press = false;
    }
    else if (kbd_data == 0xF0){
      kbd->keydown = 1;
      kbd->keycode = AM_KEY_NONE;
      is_press = false;
    }

    else {
      kbd->keydown = 1;
      kbd->keycode = kbd_data;
      is_press = false;
    }
  }

}
