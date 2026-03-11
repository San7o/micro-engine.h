// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#define MICRO_ENGINE_MEMORY_SIZE (20 * 1024 * 1024)
#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

MicroDrawCanvas canvas = {0};

bool micro_app_setup(void)
{
  int width  = 600;
  int height = 500;
  micro_platform.init("hello app", width, height);
  micro_log_init();
  micro_draw_canvas_init(&canvas, width, height, MICRO_DRAW_RGBA8);

  int x,y,n;
  const char* img_path = "utils/micro-engine-marketing.jpg";
  unsigned char *data = stbi_load(img_path, &x, &y, &n, 4);
  if (!data)
  {
    micro_log_error("Error opening image: %s", img_path);
    return false;
  }

  micro_log_info("image info: x: %d, y: %d, n: %d", x, y, n);

  MicroDrawCanvas img_canvas = {
    .data   = data,
    .width  = x,
    .height = y,
    .pixel  = MICRO_DRAW_RGBA8,
  };
  micro_draw_scaled(&img_canvas, &canvas);
  
  stbi_image_free(data);
  return true;
}

bool micro_app_update(float delta_time)
{
  (void) delta_time;
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;
  return true;
}

bool micro_app_draw(void)
{
  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();
}
