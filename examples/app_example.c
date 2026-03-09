// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

MicroPlatform set_micro_app_platform(void)
{
  return rgfw_platform;
}

bool micro_app_setup(void)
{
  micro_app_platform.init("hello app", 500, 500);
  return true;
}

bool micro_app_update(float delta_time)
{
  (void) delta_time;

  if (micro_app_platform.get_key(MICRO_KEY_ESCAPE))
    return false;
  
  return true;
}

bool micro_app_draw(void)
{
  return true;
}

void micro_app_cleanup(void)
{
  micro_app_platform.terminate();
}
