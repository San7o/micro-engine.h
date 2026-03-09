// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_APP_H
#define MICRO_APP_H

#include "micro-platform.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// You need to provide an implementation to these symbols

MicroPlatform micro_app_platform;
MicroPlatform set_micro_app_platform(void);
bool micro_app_setup(void);
bool micro_app_update(float delta_time);
bool micro_app_draw(void);
void micro_app_cleanup(void);

#ifdef MICRO_APP_MAIN

#ifdef __EMSCRIPTEN__
static void _micro_app_loop(void* prev_time_ptr)
#else
static bool _micro_app_loop(void* prev_time_ptr)
#endif
{
  long unsigned int *prev_time = ((long unsigned int*) prev_time_ptr);
  long unsigned int time = micro_app_platform.get_ticks_ms();
  long unsigned int delta = time - *prev_time;
  *prev_time = time;

  if (!micro_app_update(delta))  // user implemented
    goto error;

  if (!micro_app_draw())         // user implemented
    goto error;
    
  micro_app_platform.pool_events();

#ifdef __EMSCRIPTEN__
  return;
#else
  return true;
#endif
  
 error:
#ifdef __EMSCRIPTEN__
  return;
#else
  return false;
#endif
}

int main(void)
{
  micro_app_platform = set_micro_app_platform(); // user implemented
  
  if (!micro_app_setup())     // user implemented
    return 1;

  int fps = 60;
  long unsigned int prev_time = micro_app_platform.get_ticks_ms();
  
  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(_micro_app_loop, &prev_time, fps, true);
  #else
  (void) fps;
  while (_micro_app_loop(&prev_time)) {}
  #endif

  micro_app_cleanup();   // user implemented
  return 0;
}

#endif // MICRO_APP_MAIN

#endif // MICRO_APP_H
