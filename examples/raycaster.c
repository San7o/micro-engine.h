// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

//
// Ratcast demo
// ------------
//
// Many thanks to: https://lodev.org/cgtutor/raycasting.html
//

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

MicroDrawCanvas canvas = {0};

int world_map[MAP_WIDTH][MAP_HEIGHT] = 
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

double posX    = 22;
double posY    = 12;
double dirX    = -1;
double dirY    = 0;
double planeX  = 0;
double planeY  = 0.66;
unsigned char white[4] = {255, 255, 255, 255};
MicroDrawText info_text = {
  .text  = "move with the arrow keys / WASD",
  .x     = 10,
  .y     = 10,
  .scale = 0.3,
};

bool micro_app_setup(void)
{
  micro_platform.init("hello app", SCREEN_WIDTH, SCREEN_HEIGHT);
  micro_log_init();
  micro_draw_canvas_init(&canvas, SCREEN_WIDTH, SCREEN_HEIGHT, MICRO_DRAW_RGBA8);
  return true;
}

bool micro_app_update(float delta_time)
{
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;

  double moveSpeed = delta_time / 1000.0 * 5.0; //the constant value is in squares/second
  double rotSpeed = delta_time / 1000.0 * 3.0; //the constant value is in radians/second
  
  if(micro_platform.get_key(MICRO_KEY_UP)
     || micro_platform.get_key(MICRO_KEY_W))
  {
    if(world_map[(int)(posX + dirX * moveSpeed)][(int)(posY)] == false) posX += dirX * moveSpeed;
    if(world_map[(int)(posX)][(int)(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
  }
  if(micro_platform.get_key(MICRO_KEY_DOWN)
     || micro_platform.get_key(MICRO_KEY_S))
  {
    if(world_map[(int)(posX - dirX * moveSpeed)][(int)(posY)] == false) posX -= dirX * moveSpeed;
    if(world_map[(int)(posX)][(int)(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
  }
  if(micro_platform.get_key(MICRO_KEY_RIGHT)
     || micro_platform.get_key(MICRO_KEY_D))
  {
    double oldDirX = dirX;
    dirX = dirX * micro_cos(-rotSpeed) - dirY * micro_sin(-rotSpeed);
    dirY = oldDirX * micro_sin(-rotSpeed) + dirY * micro_cos(-rotSpeed);
    double oldPlaneX = planeX;
    planeX = planeX * micro_cos(-rotSpeed) - planeY * micro_sin(-rotSpeed);
    planeY = oldPlaneX * micro_sin(-rotSpeed) + planeY * micro_cos(-rotSpeed);
  }
  if(micro_platform.get_key(MICRO_KEY_LEFT)
     || micro_platform.get_key(MICRO_KEY_A))
  {
    double oldDirX = dirX;
    dirX = dirX * micro_cos(rotSpeed) - dirY * micro_sin(rotSpeed);
    dirY = oldDirX * micro_sin(rotSpeed) + dirY * micro_cos(rotSpeed);
    double oldPlaneX = planeX;
    planeX = planeX * micro_cos(rotSpeed) - planeY * micro_sin(rotSpeed);
    planeY = oldPlaneX * micro_sin(rotSpeed) + planeY * micro_cos(rotSpeed);
  }

  #ifndef __EMSCRIPTEN__
  micro_platform.sleep_ms(16);
  #endif
  return true;
}

unsigned char gray[4] = { 18, 18, 18, 255};

bool micro_app_draw(void)
{
  micro_draw_clear(&canvas, gray);

  for (int x = 0; x < SCREEN_WIDTH; ++x)
  {
    // Calculate ray position and direction
    double cameraX = 2 * x / (double) SCREEN_WIDTH - 1; // X coordinate in camera space
    double rayDirX = dirX + planeX * cameraX;
    double rayDirY = dirY + planeY * cameraX;
    
    int mapX = micro_floor(posX);
    int mapY = micro_floor(posY);
    double sideDistX;
    double sideDistY;

    double deltaDistX = (rayDirX == 0) ? 1e30 : micro_abs(1 / rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : micro_abs(1 / rayDirY);

    double perpWallDist;
    int stepX;
    int stepY;
    int hit = 0;
    int side;

    if (rayDirX < 0)
    {
      stepX = -1;
      sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
      stepX = 1;
      sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayDirY < 0)
    {
      stepY = -1;
      sideDistY = (posY - mapY) * deltaDistY;
    }
    else
    {
      stepY = 1;
      sideDistY = (mapY + 1.0 - posY) * deltaDistY;
    }

    // DDA

    while (hit == 0)
    {
      if (sideDistX < sideDistY)
      {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = 0;
      }
      else
      {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = 1;
      }
      if (world_map[mapX][mapY] > 0) hit = 1;
    }

    // Calculate distance projected on camera direciton
    if (side == 0) perpWallDist = (sideDistX - deltaDistX);
    else           perpWallDist = (sideDistY - deltaDistY);

    int lineHeight = micro_floor(SCREEN_HEIGHT / perpWallDist);

    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawStart < 0) drawStart = 0;
    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

    unsigned char color[4] = {0, 0, 0, 255};
    switch(world_map[mapX][mapY])
    {
    case 1:  color[0] = 255;  break; //red
    case 2:  color[1] = 255;  break; //green
    case 3:  color[2] = 255;  break; //blue
    case 4:  color[0] = 255; color[1] = 255, color[2] = 255;  break; //white
    default: color[0] = 255; color[2] = 255; break; //yellow
    }

    if (side == 1)
    {
      color[0] /= 2;
      color[1] /= 2;
      color[2] /= 2;
    }

    micro_draw_line(&canvas,
                    (Vec2) {.x = x, .y = drawStart},
                    (Vec2) {.x = x, .y = drawEnd},
                    color);
    
  }

  micro_draw_text(&canvas, info_text, white);
  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();
}
