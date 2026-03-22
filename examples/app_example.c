// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

MicroDrawCanvas canvas = {0};
unsigned char red[4]  = {255, 0, 0, 255};
unsigned char blue[4] = {0, 0, 255, 255};
MicroDrawText hello_text = {
  .text  = "hello, micro-engine!",
  .x     = 10,
  .y     = 100,
  .scale = 0.5,
};
MicroDrawText fps_text = {
  .text  = "",
  .x     = 10,
  .y     = 10,
  .scale = 0.3,
};
char fps_str[20] = {0};

bool micro_app_setup(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  
  int width  = 500;
  int height = 500;
  micro_platform.init("hello app", width, height);
  micro_log_init();
  micro_draw_canvas_init(&canvas, width, height, MICRO_DRAW_RGBA8);
  return true;
}

bool micro_app_update(float delta_time)
{
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;
  
  sprintf(fps_str, "FPS: %.2f", 1.0 / (delta_time / 1000.0));
  fps_text.text = &fps_str[0];
    
  return true;
}

bool micro_app_draw(void)
{
  micro_draw_clear(&canvas, blue);
  micro_draw_text(&canvas, hello_text, red);
  micro_draw_text(&canvas, fps_text, red);

  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();
}
