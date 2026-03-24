// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#define MICRO_MODULE_IMPLEMENTATION
#include <micro-engine/micro-module.h>

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

#define MODULE_NAME "./app_module.so"

MicroDrawCanvas canvas = {0};
MicroModule modules;

bool micro_app_setup(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  
  int width  = 1280;
  int height = 720;
  micro_platform.init("hello app", width, height);
  micro_log_init();
  micro_draw_canvas_init(&canvas, width, height, MICRO_DRAW_RGBA8);
  
  modules = micro_module_setup("app_module_name",  // symbol for name
                               "app_module_init",  // symbol for the init func
                               "app_module_exit",  // symbol for the exit func
                               false);  // create a new symbol namespace

  micro_module_init(&modules, MODULE_NAME, NULL);

  return true;
}

bool reload_pressed = false;

bool micro_app_update(float delta_time)
{
  (void) delta_time;
  
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;

  if (micro_platform.get_key(MICRO_KEY_R))
  {
    if (!reload_pressed)
    {
      reload_pressed = true;
      micro_module_exit_all(&modules, NULL);
      micro_module_init(&modules, MODULE_NAME, NULL);
    }
  }
  else
    reload_pressed = false;

  return true;
}

bool micro_app_draw(void)
{
  if (micro_module_call(&modules, "app_module", "app_module_draw", &canvas)
      != MICRO_MODULE_OK)
    return false;
  
  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);

  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();

  // Unload all modules
  micro_module_exit_all(&modules, NULL);
}
