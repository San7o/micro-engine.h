// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>

#define WIDTH  800
#define HEIGHT 600

int main(void)
{
  micro_log_init();
  micro_log_set_flags(MICRO_LOG_FLAG_LEVEL
                      | MICRO_LOG_FLAG_DATE
                      | MICRO_LOG_FLAG_TIME);
  micro_log_info("micro-engine initialized");

  RGFW_event event;
  RGFW_window* win = RGFW_createWindow("hello micro-engine",
                                       0, 0, WIDTH, HEIGHT,
                                       RGFW_windowCenter
                                       | RGFW_windowNoResize);

  MicroDrawCanvas canvas = {0};
  micro_draw_canvas_init(&canvas, WIDTH, HEIGHT, MICRO_DRAW_RGBA8);

  RGFW_surface *surface =
    RGFW_window_createSurface(win, (u8*)canvas.data, WIDTH, HEIGHT, RGFW_formatRGBA8);

  RGFW_window_setExitKey(win, RGFW_escape);

  unsigned char white[4] = {255, 255, 255, 255};
  unsigned char black[4] = {0, 0, 0, 255};
  MicroDrawText text = {
    .text  = "hello, micro-engine!",
    .x     = 100,
    .y     = 100,
    .scale = 0.5,
  };
  
	while (RGFW_window_shouldClose(win) == RGFW_FALSE)
  {
		while (RGFW_window_checkEvent(win, &event))
    {
      if (event.type == RGFW_quit)
        break;
    }
    
    micro_draw_clear(&canvas, white);
    micro_draw_text(&canvas, text, black);
    
    RGFW_window_blitSurface(win, surface);
	}

  RGFW_surface_free(surface);
  micro_draw_canvas_free(&canvas);
	RGFW_window_close(win);
  
  micro_log_close();
  return 0;
}
