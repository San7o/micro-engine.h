// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_APP_H
#define MICRO_APP_H

#include "micro-platform.h"

// You need to provide an implementation to these symbols

MicroPlatform micro_app_platform;
MicroPlatform set_micro_app_platform(void);
bool micro_app_setup(void);
bool micro_app_update(float delta_time);
bool micro_app_draw(void);
void micro_app_cleanup(void);

#ifdef MICRO_APP_MAIN

int main()
{
  micro_app_platform = set_micro_app_platform(); // user implemented
  
  if (!micro_app_setup())     // user implemented
    return 1;

  long unsigned int prev_time = micro_app_platform.get_ticks_ms();
  while(1)
  {
    long unsigned int time = micro_app_platform.get_ticks_ms();
    long unsigned int delta = time - prev_time;
    prev_time = time;
    
    if (!micro_app_update(delta))  // user implemented
      break;

    if (!micro_app_draw())         // user implemented
      break;
    
    micro_app_platform.pool_events();
  }

  micro_app_cleanup();   // user implemented
  return 0;
}

#endif // MICRO_APP_MAIN

#endif // MICRO_APP_H
