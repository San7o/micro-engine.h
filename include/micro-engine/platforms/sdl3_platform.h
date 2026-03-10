// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef SDL3_PLATFORM_H
#define SDL3_PLATFORM_H

#include "../micro-platform.h"

extern MicroPlatform sdl3_platform;

//
// Implementation
//

#ifdef SDL3_PLATFORM_IMPLEMENTATION

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

#include <assert.h>

static SDL_Window*  _sdl_window   = NULL;
static SDL_Surface* _sdl_surface  = NULL;

#ifndef MICRO_ENGINE_MEMORY_SIZE
  #define MICRO_ENGINE_MEMORY_SIZE (5 * 1024 * 1024)
#endif
char memory[MICRO_ENGINE_MEMORY_SIZE] = {0};

static bool sdl3_platform_init(const char* title, int width, int height)
{
  micro_arena_ginit(&memory[0], MICRO_ENGINE_MEMORY_SIZE);

  if (!SDL_Init(SDL_INIT_VIDEO))
    return false;

  _sdl_window = SDL_CreateWindow(title, width, height, 0);
  if (!_sdl_window)
    return false;

  _sdl_surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
  if (!_sdl_surface)
    return false;

  return true;
}

static bool sdl3_platform_terminate(void)
{
  if (_sdl_surface)
    SDL_DestroySurface(_sdl_surface);
  if (_sdl_window)
    SDL_DestroyWindow(_sdl_window);
  
  SDL_Quit();
  return true;
}

static void sdl3_pool_events(void)
{
  SDL_PumpEvents();
  return;
}

static void sdl3_platform_draw_frame(unsigned char* data, int width, int height)
{
  if (!_sdl_window || !_sdl_surface) return;
  
  assert(_sdl_surface->format == SDL_PIXELFORMAT_RGBA32);
  assert(_sdl_surface->pitch == 4 * width);
  assert(_sdl_surface->w * _sdl_surface->h == width * height);

  SDL_LockSurface(_sdl_surface);
  memcpy(_sdl_surface->pixels, data, width * height * 4);
  SDL_UnlockSurface(_sdl_surface);

  SDL_Surface *win_surface = SDL_GetWindowSurface(_sdl_window);
  SDL_BlitSurface(_sdl_surface, NULL, win_surface, NULL);
  SDL_UpdateWindowSurface(_sdl_window);
  return;
}
  
static void sdl3_platform_sleep_ms(unsigned int ms)
{
  SDL_Delay(ms);
  return;
}

static unsigned long int sdl3_platform_get_ticks_ms(void)
{
  return SDL_GetTicks();
}

static SDL_Scancode _micro_key_to_sdl_scancode(MicroKey key)
{
  switch (key)
  {
  // PRINTABLE KEYS (Physical locations)
  case MICRO_KEY_SPACE:         return SDL_SCANCODE_SPACE;
  case MICRO_KEY_APOSTROPHE:    return SDL_SCANCODE_APOSTROPHE;
  case MICRO_KEY_COMMA:         return SDL_SCANCODE_COMMA;
  case MICRO_KEY_MINUS:         return SDL_SCANCODE_MINUS;
  case MICRO_KEY_PERIOD:        return SDL_SCANCODE_PERIOD;
  case MICRO_KEY_SLASH:         return SDL_SCANCODE_SLASH;
  case MICRO_KEY_D0:            return SDL_SCANCODE_0;
  case MICRO_KEY_D1:            return SDL_SCANCODE_1;
  case MICRO_KEY_D2:            return SDL_SCANCODE_2;
  case MICRO_KEY_D3:            return SDL_SCANCODE_3;
  case MICRO_KEY_D4:            return SDL_SCANCODE_4;
  case MICRO_KEY_D5:            return SDL_SCANCODE_5;
  case MICRO_KEY_D6:            return SDL_SCANCODE_6;
  case MICRO_KEY_D7:            return SDL_SCANCODE_7;
  case MICRO_KEY_D8:            return SDL_SCANCODE_8;
  case MICRO_KEY_D9:            return SDL_SCANCODE_9;
  case MICRO_KEY_SEMICOLON:     return SDL_SCANCODE_SEMICOLON;
  case MICRO_KEY_EQUAL:         return SDL_SCANCODE_EQUALS;
  case MICRO_KEY_A:             return SDL_SCANCODE_A;
  case MICRO_KEY_B:             return SDL_SCANCODE_B;
  case MICRO_KEY_C:             return SDL_SCANCODE_C;
  case MICRO_KEY_D:             return SDL_SCANCODE_D;
  case MICRO_KEY_E:             return SDL_SCANCODE_E;
  case MICRO_KEY_F:             return SDL_SCANCODE_F;
  case MICRO_KEY_G:             return SDL_SCANCODE_G;
  case MICRO_KEY_H:             return SDL_SCANCODE_H;
  case MICRO_KEY_I:             return SDL_SCANCODE_I;
  case MICRO_KEY_J:             return SDL_SCANCODE_J;
  case MICRO_KEY_K:             return SDL_SCANCODE_K;
  case MICRO_KEY_L:             return SDL_SCANCODE_L;
  case MICRO_KEY_M:             return SDL_SCANCODE_M;
  case MICRO_KEY_N:             return SDL_SCANCODE_N;
  case MICRO_KEY_O:             return SDL_SCANCODE_O;
  case MICRO_KEY_P:             return SDL_SCANCODE_P;
  case MICRO_KEY_Q:             return SDL_SCANCODE_Q;
  case MICRO_KEY_R:             return SDL_SCANCODE_R;
  case MICRO_KEY_S:             return SDL_SCANCODE_S;
  case MICRO_KEY_T:             return SDL_SCANCODE_T;
  case MICRO_KEY_U:             return SDL_SCANCODE_U;
  case MICRO_KEY_V:             return SDL_SCANCODE_V;
  case MICRO_KEY_W:             return SDL_SCANCODE_W;
  case MICRO_KEY_X:             return SDL_SCANCODE_X;
  case MICRO_KEY_Y:             return SDL_SCANCODE_Y;
  case MICRO_KEY_Z:             return SDL_SCANCODE_Z;

  // FUNCTION KEYS
  case MICRO_KEY_ESCAPE:        return SDL_SCANCODE_ESCAPE;
  case MICRO_KEY_ENTER:         return SDL_SCANCODE_RETURN;
  case MICRO_KEY_TAB:           return SDL_SCANCODE_TAB;
  case MICRO_KEY_BACKSPACE:     return SDL_SCANCODE_BACKSPACE;
  case MICRO_KEY_INSERT:        return SDL_SCANCODE_INSERT;
  case MICRO_KEY_DELETE:        return SDL_SCANCODE_DELETE;
  case MICRO_KEY_RIGHT:         return SDL_SCANCODE_RIGHT;
  case MICRO_KEY_LEFT:          return SDL_SCANCODE_LEFT;
  case MICRO_KEY_DOWN:          return SDL_SCANCODE_DOWN;
  case MICRO_KEY_UP:            return SDL_SCANCODE_UP;
        
  // MODIFIERS
  case MICRO_KEY_LEFTSHIFT:     return SDL_SCANCODE_LSHIFT;
  case MICRO_KEY_LEFTCONTROL:   return SDL_SCANCODE_LCTRL;
  case MICRO_KEY_LEFTALT:       return SDL_SCANCODE_LALT;
  case MICRO_KEY_RIGHTSHIFT:    return SDL_SCANCODE_RSHIFT;
  case MICRO_KEY_RIGHTCONTROL:  return SDL_SCANCODE_RCTRL;
  case MICRO_KEY_RIGHTALT:      return SDL_SCANCODE_RALT;

  default:                      return SDL_SCANCODE_UNKNOWN;
  }
}

static bool sdl3_platform_get_key(MicroKey key)
{
  const bool *key_states = SDL_GetKeyboardState(NULL);
  if (!key_states) return false;
  
  const SDL_Scancode scan_code = _micro_key_to_sdl_scancode(key);
  return key_states[scan_code];
}

MicroPlatform sdl3_platform = {
  .init              = sdl3_platform_init,
  .terminate         = sdl3_platform_terminate,
  .pool_events       = sdl3_pool_events,
  .draw_frame        = sdl3_platform_draw_frame,
  .sleep_ms          = sdl3_platform_sleep_ms,
  .get_ticks_ms      = sdl3_platform_get_ticks_ms,
  .get_key           = sdl3_platform_get_key,
};

#endif // SDL3_PLATFORM_IMPLEMENTATION

#endif // SDL3_PLATFORM_H
