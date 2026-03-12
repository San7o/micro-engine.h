// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

//
// Micro engine
// ------------
//
// Core engine funtionalities.
//
// Define `MICRO_ENGINE_IMPLEMENTATION` for including the
// implementaion.
//

#ifndef MICRO_ENGINE
#define MICRO_ENGINE

//
// Memory allocation
//

// Platform abstraction
// It provides a global `micro_platform` to access it
#include "micro-platform.h"

#ifdef MICRO_ENGINE_IMPLEMENTATION
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

#define MICRO_ARENA_GLOBAL
//#define MICRO_ARENA_DEBUG
#include "micro-arena.h"

#define MICRO_LOG_MALLOC(...)   micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_LOG_FREE(...)     micro_arena_gfree(__VA_ARGS__)
#define MICRO_LOG_REALLOC(...)  micro_arena_grealloc(__VA_ARGS__)
#define MICRO_LOG_FOPEN(...)    micro_platform.open(__VA_ARGS__)
#define MICRO_LOG_FCLOSE(...)   micro_platform.close(__VA_ARGS__)
#define MICRO_LOG_FREAD(buff, size, nmemb, stream)  \
  micro_platform.read(stream, buff, (size) * (nmemb))
#define MICRO_LOG_FWRITE(buff, size, nmemb, stream)  \
  micro_platform.write(stream, buff, (size) * (nmemb))
#define MICRO_LOG_OUT(...)      micro_platform.print(__VA_ARGS__)
#include "micro-log.h"
#include "micro-la.h"

#define STBI_MALLOC(...)       micro_arena_gmalloc(__VA_ARGS__)
#define STBI_REALLOC(...)      micro_arena_grealloc(__VA_ARGS__)
#define STBI_FREE(...)         micro_arena_gfree(__VA_ARGS__)
#define MICRO_DRAW_MALLOC(...) micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_DRAW_FREE(...)   micro_arena_gfree(__VA_ARGS__)
#define MICRO_DRAW_FOPEN(...)  micro_platform.open(__VA_ARGS__)
#define MICRO_DRAW_FCLOSE(...) micro_platform.close(__VA_ARGS__)
#define MICRO_DRAW_FREAD(buff, size, nmemb, stream)  \
  micro_platform.read(stream, buff, (size) * (nmemb))
#define MICRO_DRAW_FWRITE(buff, size, nmemb, stream)  \
  micro_platform.write(stream, buff, (size) * (nmemb))
#define MICRO_DRAW_OUT(...)    micro_platform.print(__VA_ARGS__)
#include "micro-draw.h"

#endif // MICRO_ENGINE
