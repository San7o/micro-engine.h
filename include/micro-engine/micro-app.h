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

#ifdef __cplusplus
extern "C" {
#endif

// You need to provide an implementation to these symbols

bool micro_app_setup(void);
bool micro_app_update(float delta_time);
bool micro_app_draw(void);
void micro_app_cleanup(void);

#ifdef MICRO_APP_MAIN

static long unsigned int _micro_app_prev_time = 0;
#ifdef __EMSCRIPTEN__
void _micro_app_loop(void* unused)
#else
bool _micro_app_loop(void* unused)
#endif
{
  (void) unused;
  
  long unsigned int time = micro_platform.get_ticks_ms();
  long unsigned int delta = time - _micro_app_prev_time;
  _micro_app_prev_time = time;

  if (!micro_app_update(delta))  // user implemented
    goto error;

  if (!micro_app_draw())         // user implemented
    goto error;
    
  micro_platform.pool_events();

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

#ifndef WASM_PLATFORM_H
// Wasm provides its own loop in javascript

int main(void)
{
  if (!micro_app_setup())     // user implemented
    return 1;

  int fps = 60;
  _micro_app_prev_time = micro_platform.get_ticks_ms();
  
  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(_micro_app_loop, NULL, fps, true);
  #else
  (void) fps;
  while (_micro_app_loop(NULL)) {}
  #endif

  micro_app_cleanup();   // user implemented
  return 0;
}
#endif // WASM_PLATFORM_H
#endif // MICRO_APP_MAIN

#ifdef __cplusplus
}
#endif
 
#endif // MICRO_APP_H
