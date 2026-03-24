// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <stdio.h>
#include <micro-engine/micro-engine.h>

static unsigned char red[4]  = {255, 0, 0, 255};
static unsigned char blue[4] = {0, 0, 255, 255};

// If you change this, then recompile and reload, the application
// will update without ever closing.
static MicroDrawText hello_text = {
  .text  = "hello, from app_module",
  .x     = 10,
  .y     = 100,
  .scale = 0.5,
};

void hello_message(void)
{
  printf("Hello from module\n");
  return;
}

void bye_message(void)
{
  printf("Bye from module\n");
  return;
}

extern int app_module_draw(MicroDrawCanvas* canvas)
{
  micro_draw_clear(canvas, blue);
  micro_draw_text(canvas, hello_text, red);
  return 0;
}

// Symbols required by micro_module

const char app_module_name[] = "app_module";

extern int app_module_init(void* arg)
{
  (void) arg;
  hello_message();
  return 0;
}

extern int app_module_exit(void* arg)
{
  (void) arg;
  
  bye_message();
  return 0;
}
