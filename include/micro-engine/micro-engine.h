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

#define MICRO_HEADERS_MALLOC(...)   micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_HEADERS_FREE(...)     micro_arena_gfree(__VA_ARGS__)
#define MICRO_HEADERS_REALLOC(...)  micro_arena_grealloc(__VA_ARGS__)
#define MICRO_HEADERS_CALLOC(...)   micro_arena_calloc(__VA_ARGS__)
  
#define MICRO_HEADERS_FOPEN(...)    micro_platform.open(__VA_ARGS__)
#define MICRO_HEADERS_FCLOSE(...)   micro_platform.close(__VA_ARGS__)
#define MICRO_HEADERS_FREAD(buff, size, nmemb, stream)  \
  micro_platform.read(stream, buff, (size) * (nmemb))
#define MICRO_HEADERS_FWRITE(buff, size, nmemb, stream)    \
  micro_platform.write(stream, buff, (size) * (nmemb))
#define MICRO_HEADERS_OUT(...)      micro_platform.print(__VA_ARGS__)

#define STBI_MALLOC(...)       MICRO_HEADERS_MALLOC(__VA_ARGS__)
#define STBI_REALLOC(...)      MICRO_HEADERS_REALLOC(__VA_ARGS__)
#define STBI_FREE(...)         MICRO_HEADERS_FREE(__VA_ARGS__)
  
#define MICRO_ARENA_GLOBAL
//#define MICRO_ARENA_DEBUG
#include "micro-arena.h"
#include "micro-log.h"
#include "micro-la.h"
#include "micro-draw.h"

#ifdef __cplusplus
}
#endif

#endif // MICRO_ENGINE
