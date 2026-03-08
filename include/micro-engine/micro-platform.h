// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_PLATFORM_H
#define MICRO_PLATFORM_H

#include <stdbool.h>

typedef struct MicroPlatform {
  
  bool (*init)(void);
  bool (*terminate)(void);
  
  void (*draw_frame)(unsigned char* data, int width, int height);
  
  void (*sleep_ms)(unsigned int ms);
  unsigned int (*get_ticks_ms)(void);
  bool (*get_key)(unsigned char key);
  void (*set_window_title)(const char* title); // optional
  
} MicroPlatform;

#endif // MICRO_PLATFORM_H
