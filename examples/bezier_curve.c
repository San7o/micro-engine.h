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

float t = 0;
Vec2 point_a = {
  .x = 100,
  .y = 300,
};
Vec2 point_b = {
  .x = 200,
  .y = 100,
};
Vec2 point_c = {
  .x = 400,
  .y = 400,
};

int lerp(int a, int b, float t)
{
  return (b - a) * t + a;
}

bool micro_app_setup(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  
  int width  = 500;
  int height = 500;
  micro_platform.init("bezier curve", width, height);
  micro_log_init();
  micro_draw_canvas_init(&canvas, width, height, MICRO_DRAW_RGBA8);
  return true;
}

bool micro_app_update(float delta_time)
{
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;

  t += delta_time / 1000;

  if (t >= 1.0)
    t = 0;
  
  return true;
}

bool micro_app_draw(void)
{
  unsigned char black[] = {0, 0, 0, 255};
  unsigned char white[] = {255, 255, 255, 255};
  unsigned char red[] = {255, 0, 0, 255};
  micro_draw_clear(&canvas, black);

  Vec2 lerp_ab = {
    .x = lerp(point_a.x, point_b.x, t),
    .y = lerp(point_a.y, point_b.y, t),
  };
  micro_draw_line(&canvas, point_a, point_b, white);
  micro_draw_pixel(&canvas, lerp_ab, red);
  
  Vec2 lerp_bc = {
    .x = lerp(point_b.x, point_c.x, t),
    .y = lerp(point_b.y, point_c.y, t),
  };
  micro_draw_line(&canvas, point_b, point_c, white);
  micro_draw_pixel(&canvas, lerp_bc, red);

  micro_draw_line(&canvas, lerp_ab, lerp_bc, white);  

  Vec2 lerp_lerp = {
    .x = lerp(lerp_ab.x, lerp_bc.x, t),
    .y = lerp(lerp_ab.y, lerp_bc.y, t),
  };
  micro_draw_pixel(&canvas, lerp_lerp, red);

  float step_size = 0.01;
  for (float i = 0.0; i < t; i += step_size)
  {
    Vec2 lerp_ab = {
      .x = lerp(point_a.x, point_b.x, i),
      .y = lerp(point_a.y, point_b.y, i),
    };
    Vec2 lerp_bc = {
      .x = lerp(point_b.x, point_c.x, i),
      .y = lerp(point_b.y, point_c.y, i),
    };
    Vec2 lerp_lerp = {
      .x = lerp(lerp_ab.x, lerp_bc.x, i),
      .y = lerp(lerp_ab.y, lerp_bc.y, i),
    };
    micro_draw_pixel(&canvas, lerp_lerp, red);
  }
  
  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();
}
