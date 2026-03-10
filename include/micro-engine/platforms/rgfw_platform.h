// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef RGFW_PLATFORM_H
#define RGFW_PLATFORM_H

#include "../micro-platform.h"

extern MicroPlatform rgfw_platform;

//
// Implementation
//

#ifdef RGFW_PLATFORM_IMPLEMENTATION

#define RGFW_IMPLEMENTATION
#include "../external/RGFW.h"

#include <assert.h>
#include <time.h>
#include <sys/time.h>

static RGFW_window*   _rgfw_window   = NULL;
static RGFW_surface*  _rgfw_surface  = NULL;
static struct timeval _rgfw_time;

#ifndef MICRO_ENGINE_MEMORY_SIZE
  #define MICRO_ENTINE_MEMORY_SIZE (5 * 1024 * 1024)
#endif
char buff[MICRO_ENGINE_MEMORY_SIZE];

static bool rgfw_platform_init(const char* title, int width, int height)
{
  micro_arena_ginit(&buff[0], MICRO_ENGINE_MEMORY_SIZE);

  int flags = RGFW_windowNoResize | RGFW_windowCenter | RGFW_windowFloating;
  _rgfw_window = RGFW_createWindow(title, 0, 0, width, height, flags);

  unsigned char* data = RGFW_alloc(width * height * 4);
  _rgfw_surface = RGFW_window_createSurface(_rgfw_window, data, width,
                                            height, RGFW_formatRGBA8);
  return true;
}

static bool rgfw_platform_terminate(void)
{
  if (_rgfw_surface)
    RGFW_surface_free(_rgfw_surface);
  if (_rgfw_window)
    RGFW_window_close(_rgfw_window);

  return true;
}

static void rgfw_pool_events(void)
{
  RGFW_pollEvents();
  return;
}

static void rgfw_platform_draw_frame(unsigned char* data, int width, int height)
{
  if (!_rgfw_window || !_rgfw_surface) return;

  // TODO: assertions
  
  memcpy(_rgfw_surface->data, data, width * height * 4);

  RGFW_window_blitSurface(_rgfw_window, _rgfw_surface);
  return;
}
  
static void rgfw_platform_sleep_ms(unsigned int ms)
{
  struct timespec time = {0};
  time.tv_nsec = ms * 1000000;
  nanosleep(&time, NULL);
  return;
}

static unsigned long int rgfw_platform_get_ticks_ms(void)
{
  gettimeofday(&_rgfw_time, NULL);
  return (((unsigned long)_rgfw_time.tv_sec)*1000)+(_rgfw_time.tv_usec/1000);
}

static RGFW_key _micro_key_to_rgfw_scancode(MicroKey key)
{
  switch (key)
  {
  // PRINTABLE KEYS
  case MICRO_KEY_SPACE:         return RGFW_space;
  case MICRO_KEY_APOSTROPHE:    return RGFW_apostrophe;
  case MICRO_KEY_COMMA:         return RGFW_comma;
  case MICRO_KEY_MINUS:         return RGFW_minus;
  case MICRO_KEY_PERIOD:        return RGFW_period;
  case MICRO_KEY_SLASH:         return RGFW_slash;
  case MICRO_KEY_D0:            return RGFW_0;
  case MICRO_KEY_D1:            return RGFW_1;
  case MICRO_KEY_D2:            return RGFW_2;
  case MICRO_KEY_D3:            return RGFW_3;
  case MICRO_KEY_D4:            return RGFW_4;
  case MICRO_KEY_D5:            return RGFW_5;
  case MICRO_KEY_D6:            return RGFW_6;
  case MICRO_KEY_D7:            return RGFW_7;
  case MICRO_KEY_D8:            return RGFW_8;
  case MICRO_KEY_D9:            return RGFW_9;
  case MICRO_KEY_SEMICOLON:     return RGFW_semicolon;
  case MICRO_KEY_EQUAL:         return RGFW_equals;
  case MICRO_KEY_A:             return RGFW_a;
  case MICRO_KEY_B:             return RGFW_b;
  case MICRO_KEY_C:             return RGFW_c;
  case MICRO_KEY_D:             return RGFW_d;
  case MICRO_KEY_E:             return RGFW_e;
  case MICRO_KEY_F:             return RGFW_f;
  case MICRO_KEY_G:             return RGFW_g;
  case MICRO_KEY_H:             return RGFW_h;
  case MICRO_KEY_I:             return RGFW_i;
  case MICRO_KEY_J:             return RGFW_j;
  case MICRO_KEY_K:             return RGFW_k;
  case MICRO_KEY_L:             return RGFW_l;
  case MICRO_KEY_M:             return RGFW_m;
  case MICRO_KEY_N:             return RGFW_n;
  case MICRO_KEY_O:             return RGFW_o;
  case MICRO_KEY_P:             return RGFW_p;
  case MICRO_KEY_Q:             return RGFW_q;
  case MICRO_KEY_R:             return RGFW_r;
  case MICRO_KEY_S:             return RGFW_s;
  case MICRO_KEY_T:             return RGFW_t;
  case MICRO_KEY_U:             return RGFW_u;
  case MICRO_KEY_V:             return RGFW_v;
  case MICRO_KEY_W:             return RGFW_w;
  case MICRO_KEY_X:             return RGFW_x;
  case MICRO_KEY_Y:             return RGFW_y;
  case MICRO_KEY_Z:             return RGFW_z;
  case MICRO_KEY_LEFTBRACKET:   return RGFW_bracket;
  case MICRO_KEY_BACKSLASH:     return RGFW_backSlash;
  case MICRO_KEY_RIGHTBRACKET:  return RGFW_closeBracket;
  case MICRO_KEY_GRAVEACCENT:   return RGFW_backtick;
  case MICRO_KEY_WORLD1:        return RGFW_world1;
  case MICRO_KEY_WORLD2:        return RGFW_world2;

  // FUNCTION KEYS
  case MICRO_KEY_ESCAPE:        return RGFW_escape;
  case MICRO_KEY_ENTER:         return RGFW_return;
  case MICRO_KEY_TAB:           return RGFW_tab;
  case MICRO_KEY_BACKSPACE:     return RGFW_backSpace;
  case MICRO_KEY_INSERT:        return RGFW_insert;
  case MICRO_KEY_DELETE:        return RGFW_delete;
  case MICRO_KEY_RIGHT:         return RGFW_right;
  case MICRO_KEY_LEFT:          return RGFW_left;
  case MICRO_KEY_DOWN:          return RGFW_down;
  case MICRO_KEY_UP:            return RGFW_up;
  case MICRO_KEY_PAGEUP:        return RGFW_pageUp;
  case MICRO_KEY_PAGEDOWN:      return RGFW_pageDown;
  case MICRO_KEY_HOME:          return RGFW_home;
  case MICRO_KEY_END:           return RGFW_end;
  case MICRO_KEY_CAPSLOCK:      return RGFW_capsLock;
  case MICRO_KEY_SCROLLLOCK:    return RGFW_scrollLock;
  case MICRO_KEY_NUMLOCK:       return RGFW_numLock;
  case MICRO_KEY_PRINTSCREEN:   return RGFW_printScreen;
  case MICRO_KEY_PAUSE:         return RGFW_pause;
  case MICRO_KEY_F1:            return RGFW_F1;
  case MICRO_KEY_F2:            return RGFW_F2;
  case MICRO_KEY_F3:            return RGFW_F3;
  case MICRO_KEY_F4:            return RGFW_F4;
  case MICRO_KEY_F5:            return RGFW_F5;
  case MICRO_KEY_F6:            return RGFW_F6;
  case MICRO_KEY_F7:            return RGFW_F7;
  case MICRO_KEY_F8:            return RGFW_F8;
  case MICRO_KEY_F9:            return RGFW_F9;
  case MICRO_KEY_F10:           return RGFW_F10;
  case MICRO_KEY_F11:           return RGFW_F11;
  case MICRO_KEY_F12:           return RGFW_F12;
  case MICRO_KEY_F13:           return RGFW_F13;
  case MICRO_KEY_F14:           return RGFW_F14;
  case MICRO_KEY_F15:           return RGFW_F15;
  case MICRO_KEY_F16:           return RGFW_F16;
  case MICRO_KEY_F17:           return RGFW_F17;
  case MICRO_KEY_F18:           return RGFW_F18;
  case MICRO_KEY_F19:           return RGFW_F19;
  case MICRO_KEY_F20:           return RGFW_F20;
  case MICRO_KEY_F21:           return RGFW_F21;
  case MICRO_KEY_F22:           return RGFW_F22;
  case MICRO_KEY_F23:           return RGFW_F23;
  case MICRO_KEY_F24:           return RGFW_F24;
  case MICRO_KEY_F25:           return RGFW_F25;

  // KEYPAD
  case MICRO_KEY_KP0:           return RGFW_kp0;
  case MICRO_KEY_KP1:           return RGFW_kp1;
  case MICRO_KEY_KP2:           return RGFW_kp2;
  case MICRO_KEY_KP3:           return RGFW_kp3;
  case MICRO_KEY_KP4:           return RGFW_kp4;
  case MICRO_KEY_KP5:           return RGFW_kp5;
  case MICRO_KEY_KP6:           return RGFW_kp6;
  case MICRO_KEY_KP7:           return RGFW_kp7;
  case MICRO_KEY_KP8:           return RGFW_kp8;
  case MICRO_KEY_KP9:           return RGFW_kp9;
  case MICRO_KEY_KPDECIMAL:     return RGFW_kpPeriod;
  case MICRO_KEY_KPDIVIDE:      return RGFW_kpSlash;
  case MICRO_KEY_KPMULTIPLY:    return RGFW_kpMultiply;
  case MICRO_KEY_KPSUBTRACT:    return RGFW_kpMinus;
  case MICRO_KEY_KPADD:         return RGFW_kpPlus;
  case MICRO_KEY_KPENTER:       return RGFW_kpReturn;
  case MICRO_KEY_KPEQUAL:       return RGFW_kpEqual;

  // MODIFIERS
  case MICRO_KEY_LEFTSHIFT:     return RGFW_shiftL;
  case MICRO_KEY_LEFTCONTROL:   return RGFW_controlL;
  case MICRO_KEY_LEFTALT:       return RGFW_altL;
  case MICRO_KEY_LEFTSUPER:     return RGFW_superL;
  case MICRO_KEY_RIGHTSHIFT:    return RGFW_shiftR;
  case MICRO_KEY_RIGHTCONTROL:  return RGFW_controlR;
  case MICRO_KEY_RIGHTALT:      return RGFW_altR;
  case MICRO_KEY_RIGHTSUPER:    return RGFW_superR;
  case MICRO_KEY_MENU:          return RGFW_menu;

  default:                      return RGFW_keyNULL;
  }
}

static bool rgfw_platform_get_key(MicroKey key)
{
  RGFW_key rgfw_key = _micro_key_to_rgfw_scancode(key);
  return RGFW_window_isKeyDown(_rgfw_window, rgfw_key);
}

MicroPlatform rgfw_platform = {
  .init              = rgfw_platform_init,
  .terminate         = rgfw_platform_terminate,
  .pool_events       = rgfw_pool_events,
  .draw_frame        = rgfw_platform_draw_frame,
  .sleep_ms          = rgfw_platform_sleep_ms,
  .get_ticks_ms      = rgfw_platform_get_ticks_ms,
  .get_key           = rgfw_platform_get_key,
};

#endif // RGFW_PLATFORM_IMPLEMENTATION

#endif // RGFW_PLATFORM_H
