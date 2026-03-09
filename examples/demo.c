// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>
#define SDL3_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/sdl3_platform.h>

#define WIDTH  800
#define HEIGHT 600

int main(void)
{
  sdl3_platform.init("micro-engine", WIDTH, HEIGHT);

  micro_log_init();
  micro_log_set_flags(MICRO_LOG_FLAG_LEVEL
                      | MICRO_LOG_FLAG_DATE
                      | MICRO_LOG_FLAG_TIME);
  micro_log_info("micro-engine initialized");

  MicroDrawCanvas canvas = {0};
  micro_draw_canvas_init(&canvas, WIDTH, HEIGHT, MICRO_DRAW_RGBA8);

  unsigned char white[4] = {255, 255, 255, 255};
  unsigned char black[4] = {0, 0, 0, 255};
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

  unsigned long int prev_time = 0;
  unsigned long int time;
	while (1)
  {
    sdl3_platform.pool_events();
		if (sdl3_platform.get_key(MICRO_KEY_ESCAPE))
      break;

    time = sdl3_platform.get_ticks_ms();
    unsigned long int delta = time - prev_time;
    prev_time = time;
    char fps_str[10] = {0};
    sprintf(fps_str, "fps %.2f", 1.0 / (delta / 1000.0));
    fps_text.text = &fps_str[0];
    
    micro_draw_clear(&canvas, white);
    micro_draw_text(&canvas, hello_text, black);
    micro_draw_text(&canvas, fps_text, black);

    sdl3_platform.draw_frame(canvas.data, canvas.width, canvas.height);
    sdl3_platform.sleep_ms(16);
	}

  micro_draw_canvas_free(&canvas);
  micro_log_close();
  sdl3_platform.terminate();
  return 0;
}
