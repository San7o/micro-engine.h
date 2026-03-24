// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

//
// Micro engine
// ------------
//
// Core engine funtionalities, with some glue.
//
// Define `MICRO_ENGINE_IMPLEMENTATION` for including the
// implementaion.
//

#ifndef MICRO_ENGINE
#define MICRO_ENGINE

#ifdef __cplusplus
extern "C" {
#endif

// Platform abstraction
// It provides a global `micro_platform` to access it
#include "micro-platform.h"

#ifdef MICRO_ENGINE_IMPLEMENTATION

  // Core headers
  #define MICRO_ARENA_IMPLEMENTATION
  #define MICRO_LOG_IMPLEMENTATION
  #define MICRO_DRAW_PPM
  #define MICRO_DRAW_IMPLEMENTATION

  // Provide a memset implementation for -nostdlib environments
  // This is required since the compiler may generate a call to
  // memset
  __attribute__((weak))
  void* memset(void* dest, int c, unsigned long n)
  {
    volatile unsigned char* p = (volatile unsigned char*)dest;
    while (n--) {
      *p++ = (unsigned char)c;
    }
    return dest;
  }
#endif

#ifndef MICRO_ENGINE_MEMORY_SIZE
  #define MICRO_ENGINE_MEMORY_SIZE (4 * 1024 * 1024) // bytes
#endif
#define MICRO_ENGINE_MALLOC(...)   micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_ENGINE_FREE(...)     micro_arena_gfree(__VA_ARGS__)
#define MICRO_ENGINE_REALLOC(...)  micro_arena_grealloc(__VA_ARGS__)
  
#define MICRO_ENGINE_FOPEN(...)    micro_platform.open(__VA_ARGS__)
#define MICRO_ENGINE_FCLOSE(...)   micro_platform.close(__VA_ARGS__)
#define MICRO_ENGINE_FREAD(buff, size, nmemb, stream)  \
  micro_platform.read(stream, buff, (size) * (nmemb))
#define MICRO_ENGINE_FWRITE(buff, size, nmemb, stream)    \
  micro_platform.write(stream, buff, (size) * (nmemb))
#define MICRO_ENGINE_OUT(...)      micro_platform.print(__VA_ARGS__)
#define MICRO_ARENA_GLOBAL
//#define MICRO_ARENA_DEBUG
#include "micro-arena.h"

#define MICRO_LOG_MALLOC(...)   MICRO_ENGINE_MALLOC(__VA_ARGS__)
#define MICRO_LOG_FREE(...)     MICRO_ENGINE_FREE(__VA_ARGS__)
#define MICRO_LOG_REALLOC(...)  MICRO_ENGINE_REALLOC(__VA_ARGS__)
#define MICRO_LOG_FOPEN(...)    MICRO_ENGINE_FOPEN(__VA_ARGS__)
#define MICRO_LOG_FCLOSE(...)   MICRO_ENGINE_FCLOSE(__VA_ARGS__)
#define MICRO_LOG_FREAD(...)    MICRO_ENGINE_FREAD(__VA_ARGS__)
#define MICRO_LOG_FWRITE(...)   MICRO_ENGINE_FWRITE(__VA_ARGS__)
#define MICRO_LOG_OUT(...)      MICRO_ENGINE_OUT(__VA_ARGS__)
#include "micro-log.h"
  
#include "micro-la.h"

#define STBI_MALLOC(...)       MICRO_ENGINE_MALLOC(__VA_ARGS__)
#define STBI_REALLOC(...)      MICRO_ENGINE_REALLOC(__VA_ARGS__)
#define STBI_FREE(...)         MICRO_ENGINE_FREE(__VA_ARGS__)
#define MICRO_DRAW_MALLOC(...) MICRO_ENGINE_MALLOC(__VA_ARGS__)
#define MICRO_DRAW_FREE(...)   MICRO_ENGINE_FREE(__VA_ARGS__)
#define MICRO_DRAW_FOPEN(...)  MICRO_ENGINE_FOPEN(__VA_ARGS__)
#define MICRO_DRAW_FCLOSE(...) MICRO_ENGINE_FCLOSE(__VA_ARGS__)
#define MICRO_DRAW_FREAD(...)  MICRO_ENGINE_FREAD(__VA_ARGS__)
#define MICRO_DRAW_FWRITE(...) MICRO_ENGINE_FWRITE(__VA_ARGS__)
#define MICRO_DRAW_OUT(...)    MICRO_ENGINE_OUT(__VA_ARGS__)
#include "micro-draw.h"

#ifdef __cplusplus
}
#endif

#endif // MICRO_ENGINE
