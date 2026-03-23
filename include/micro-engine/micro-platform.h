// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_PLATFORM_H
#define MICRO_PLATFORM_H

#include "micro-key.h"
#include "micro-types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MICRO_FILE_MODE_READ   0
#define MICRO_FILE_MODE_WRITE  1
typedef int MicroFileMode;

#define MICRO_PLATFORM_FEATURE_WINDOW   (1 << 0)
#define MICRO_PLATFORM_FEATURE_INPUT    (1 << 1)
#define MICRO_PLATFORM_FEATURE_FS       (1 << 2)
// Features flag bitfield
typedef unsigned long long MicroPlatformFeatures;
  
typedef struct MicroPlatform {

  MicroPlatformFeatures (*get_features)(void);
  bool (*init)(const char* title, int width, int height);
  bool (*terminate)(void);
  
  void (*pool_events)(void);
  void (*sleep_ms)(unsigned int ms);
  unsigned long int (*get_ticks_ms)(void);
  bool (*get_key)(MicroKey key);

  // I/O
  int (*print)(const char* msg);
  void (*draw_frame)(unsigned char* data, int width, int height);
  // File I/O
  void* (*open)(const char* path, MicroFileMode mode);
  bool  (*close)(void* handle);
  size_t (*read)(void* handle, void* buffer, size_t size);
  size_t (*write)(void* handle, void* buffer, size_t size);
  
} MicroPlatform;

// Global platform
extern MicroPlatform micro_platform;

#ifdef __cplusplus
}
#endif

#endif // MICRO_PLATFORM_H
