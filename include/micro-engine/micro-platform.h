// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_PLATFORM_H
#define MICRO_PLATFORM_H

#include "micro-key.h"

#include <stdbool.h>

typedef struct MicroPlatform {
  
  bool (*init)(const char* title, int width, int height);
  bool (*terminate)(void);

  void (*pool_events)(void);
  void (*draw_frame)(unsigned char* data, int width, int height);
  
  void (*sleep_ms)(unsigned int ms);
  unsigned long int (*get_ticks_ms)(void);
  bool (*get_key)(MicroKey key);
  
} MicroPlatform;

#endif // MICRO_PLATFORM_H
