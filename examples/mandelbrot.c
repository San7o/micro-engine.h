// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>
#include <micro-engine/platforms/rgfw_platform.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define WIDTH  1000
#define HEIGHT 700
MicroDrawCanvas canvas = {0};

#define MANDELBROT_MAX_ITERATIONS 50

// Get the number of iterations of a point in the Mandelbrot set.
int mandelbrot(int x, int y, int width, int height)
{
  double x_norm = (x / (double) width) * 2.47 - 2;
  double y_norm = (y / (double) height) * 2.24 - 1.12;

  int it = 0;
  double x_it = 0.0;
  double y_it = 0.0;
  while (x_it * x_it + y_it * y_it <= 2*2
         && it < MANDELBROT_MAX_ITERATIONS)
  {
    double tmp = x_it * x_it - y_it * y_it + x_norm;
    y_it = 2 * x_it * y_it + y_norm;
    x_it = tmp;
    it++;
  }
  
  return it;
}

bool micro_app_setup(int argc, char** argv)
{
  (void) argc;
  (void) argv;
  
  micro_platform.init("mandelbrot", WIDTH, HEIGHT);
  micro_log_init();
  micro_draw_canvas_init(&canvas, WIDTH, HEIGHT, MICRO_DRAW_RGBA8);
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
  for (int x = 0; x < WIDTH; ++x)
    for (int y = 0; y < HEIGHT; ++y)
    {
      int its = mandelbrot(x, y, WIDTH, HEIGHT);
      unsigned char color[4];

      float t = (float)its / MANDELBROT_MAX_ITERATIONS;    // Normalize
      color[0] = (unsigned char)((1 - t) * 0 + t * 255);   // R
      color[1] = (unsigned char)((1 - t) * 7 + t * 170);   // G
      color[2] = (unsigned char)((1 - t) * 100 + t * 0);   // B
      
      micro_draw_pixel(&canvas, (Vec2){ .x = x, .y = y }, color);
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
