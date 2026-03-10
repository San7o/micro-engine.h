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

#ifdef MICRO_ENGINE_IMPLEMENTATION
  #define MICRO_ARENA_IMPLEMENTATION
  #define MICRO_LOG_IMPLEMENTATION
  #define MICRO_DRAW_IMPLEMENTATION

  // External
  #define STB_IMAGE_IMPLEMENTATION
  #define STB_TRUETYPE_IMPLEMENTATION
#endif

//
// Memory allocation
//

#define MICRO_ARENA_GLOBAL
//#define MICRO_ARENA_DEBUG
#include "micro-arena.h"

#define MICRO_LOG_MALLOC(...)  micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_LOG_FREE(...)    micro_arena_gfree(__VA_ARGS__)
#define MICRO_DRAW_MALLOC(...) micro_arena_gmalloc(__VA_ARGS__)
#define MICRO_DRAW_FREE(...)   micro_arena_gfree(__VA_ARGS__)
#define STBI_MALLOC(...)       micro_arena_gmalloc(__VA_ARGS__)
#define STBI_REALLOC(...)      micro_arena_grealloc(__VA_ARGS__)
#define STBI_FREE(...)         micro_arena_gfree(__VA_ARGS__)

#ifndef MICRO_ENGINE_MEMORY_SIZE
  #define MICRO_ENGINE_MEMORY_SIZE (4 * 1024 * 1024) // bytes
#endif


#include "micro-log.h"
#define MICRO_LA_IMPLEMENTATION
#include "micro-la.h"
#include "micro-draw.h"
#include "micro-platform.h"

// External
#include "external/stb_image.h"
#include "external/stb_truetype.h"

#endif // MICRO_ENGINE
