// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_PLATFORM_H
#define MICRO_PLATFORM_H

#include "micro-key.h"
#include "micro-types.h"

typedef enum MicroFileMode {
  MICRO_FILE_MODE_READ,
  MICRO_FILE_MODE_WRITE,
} MicroFileMode;

typedef struct MicroPlatform {
  
  bool (*init)(const char* title, int width, int height);
  bool (*terminate)(void);
  
  void (*pool_events)(void);
  void (*sleep_ms)(unsigned int ms);
  unsigned long int (*get_ticks_ms)(void);
  bool (*get_key)(MicroKey key);

  // I/O
  void (*print)(const char* msg);
  void (*draw_frame)(unsigned char* data, int width, int height);
  // File I/O
  void* (*open)(const char* path, MicroFileMode mode);
  void  (*close)(void* handle);
  size_t (*read)(void* handle, void* buffer, size_t size);
  size_t (*write)(void* handle, void* buffer, size_t size);
  
} MicroPlatform;

// Global platform
extern MicroPlatform micro_platform;

#endif // MICRO_PLATFORM_H
