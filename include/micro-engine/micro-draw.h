//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-draw.h
// ============
//
// Header-only software renderer in C99.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Features
// --------
//
//  - lines
//  - rectangles
//  - circles
//  - triangles
//  - grids
//  - text
//  - color RGBA, Black&White, easily add more formats
//  - PPM file reading and writing
//  - resize
//  - overlap
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_DRAW_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_DRAW_IMPLEMENTATION
//   #include "micro-draw.h"
//
// You can tune the library by #defining certain values. See the
// "Config" comments under "Configuration" below.
//
// To enable PPM related functions, you need to #define MIRO_DRAW_PPM.
// More info: https://en.wikipedia.org/wiki/Netpbm
//
// The usage is quite straight forward: you supply a data buffer to a
// micro-draw.h function which will fill the pixels accordingly. For
// example, you can use this buffer to render a frame on screen, or to
// save it as an image file, or do whatever you want.
//
// Check out some examples in the `test` directory.
//
//
// Code
// ----
//
// The official git repository of micro-draw.h is hosted at:
//
//     https://github.com/San7o/micro-draw.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//
//
// TODO
// ----
//
//  - Implement micro_draw_from_ppm
//

#ifndef MICRO_DRAW
#define MICRO_DRAW

#define MICRO_DRAW_MAJOR 0
#define MICRO_DRAW_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Configuration
//

// Config: enable PPM format reading / writing with MICRO_DRAW_PPM
#if 0
  #define MICRO_DRAW_PPM
#endif

// Config: Prefix for all functions
// For function inlining, set this to `static inline` and then define
// the implementation in all the files
#ifndef MICRO_DRAW_DEF
  #define MICRO_DRAW_DEF extern
#endif

#ifndef MICRO_DRAW_MALLOC
  #define MICRO_DRAW_MALLOC malloc
#endif

#ifndef MICRO_DRAW_FREE
  #define MICRO_DRAW_FREE free
#endif
  
//
// Types
//

typedef enum {
  MICRO_DRAW_RGBA8 = 0,
  MICRO_DRAW_BLACK_WHITE,
  _MICRO_DRAW_PIXEL_MAX,
} MicroDrawPixel;

typedef enum {
  MICRO_DRAW_OK = 0,
  MICRO_DRAW_ERROR_OPEN_FILE,
  MICRO_DRAW_ERROR_INVALID_MAGIC_NUMBER,
  MICRO_DRAW_ERROR_INVALID_IMAGE_SIZE,
  _MICRO_DRAW_ERROR_MAX,
} MicroDrawError;

#define MICRO_DRAW_FONT_HEIGHT 6
#define MICRO_DRAW_FONT_WIDTH 5
extern unsigned char
micro_draw_font[128][MICRO_DRAW_FONT_HEIGHT][MICRO_DRAW_FONT_WIDTH];
// Default character size in pixel
#define MICRO_DRAW_CHARACTER_PIXELS_X 50
#define MICRO_DRAW_CHARACTER_PIXELS_Y 50

typedef struct {
  unsigned char* data;
  int width;
  int height;
  MicroDrawPixel pixel;
} MicroDrawCanvas;

typedef struct {
  char* text;
  int x;
  int y;
  float scale;
} MicroDrawText;
  
#ifndef MICRO_LA

typedef struct {
  int c_x; int c_y;
  int r;
} Circle;

typedef struct {
  int a_x; int a_y;
  int b_x; int b_y;
  int c_x; int c_y;
  int d_x; int d_y;
} Rect;
  
typedef struct {
  int a_x; int a_y;
  int b_x; int b_y;
  int c_x; int c_y;
} Triangle;
  
typedef struct {
  int x;
  int y;
} Vec2;
  
#else

typedef Recti       Rect;
typedef Circlei     Circle;
typedef Trianglei   Triangle;
typedef Vec2i       Vec2;
  
#endif // MICRO_LA
  
//
// Function declarations
//

// Canvas ------------------------------------------------------------

MICRO_DRAW_DEF void
micro_draw_canvas_init(MicroDrawCanvas *canvas,
                       int width, int height,
                       MicroDrawPixel pixel);

MICRO_DRAW_DEF void micro_draw_canvas_free(MicroDrawCanvas *canvas);
  
// Color -------------------------------------------------------------
  
// Return the number of channels of a pixel type
MICRO_DRAW_DEF unsigned int micro_draw_get_channels(MicroDrawPixel pixel);

// Returns the number of bytes of a single channel if a pixel type
MICRO_DRAW_DEF unsigned int micro_draw_get_channel_size(MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_color_to_rgba8(unsigned char* color_src, MicroDrawPixel pixel_src,
                          unsigned char color_dest[4]);

MICRO_DRAW_DEF void
micro_draw_color_from_rgba8(unsigned char color_src[4],
                            unsigned char *color_dest, MicroDrawPixel pixel_dest);
  
// Set the memory pointed by [color_dest] with the converted [pixel_dest]
// color values from [pixel_src] in [color_src].
// The user must ensure that enough memory is available for storing
// the colors.
MICRO_DRAW_DEF void
micro_draw_color_convert(unsigned char *color_src, MicroDrawPixel pixel_src,
                         unsigned char *color_dest, MicroDrawPixel pixel_dest);
  
MICRO_DRAW_DEF unsigned char*
micro_draw_color_from_rgba(unsigned char* color,
                           MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_get_color(MicroDrawCanvas *canvas,
                     Vec2 pos,
                     unsigned char** color);

// Primitives --------------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_pixel(MicroDrawCanvas *canvas,
                 Vec2 pos,
                 unsigned char *color);

MICRO_DRAW_DEF void
micro_draw_line(MicroDrawCanvas *canvas,
                Vec2 a,
                Vec2 b,
                unsigned char* color);

MICRO_DRAW_DEF void
micro_draw_fill_rect(MicroDrawCanvas *canvas,
                     Rect rect,
                     unsigned char *color);

MICRO_DRAW_DEF void
micro_draw_fill_circle(MicroDrawCanvas *canvas,
                       Circle circle,
                       unsigned char *color);

MICRO_DRAW_DEF void
micro_draw_fill_triangle(MicroDrawCanvas *canvas,
                         Triangle triangle,
                         unsigned char *color);

MICRO_DRAW_DEF void
micro_draw_grid(MicroDrawCanvas *canvas,
                int columns, int rows,
                unsigned char* color);

MICRO_DRAW_DEF void
micro_draw_clear(MicroDrawCanvas *canvas,
                 unsigned char *color);

// Transformations ---------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_scaled(MicroDrawCanvas *source_canvas,
                  MicroDrawCanvas *dest_canvas);

MICRO_DRAW_DEF void
micro_draw_overlap(MicroDrawCanvas *source_canvas,
                   MicroDrawCanvas *dest_canvas,
                   Vec2 offset);
  
// Text --------------------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_text(MicroDrawCanvas *canvas,
                MicroDrawText text,
                unsigned char* color);

// PPM ---------------------------------------------------------------
  
#ifdef MICRO_DRAW_PPM

MICRO_DRAW_DEF MicroDrawError
micro_draw_to_ppm(const char *filename, MicroDrawCanvas *canvas);


MICRO_DRAW_DEF MicroDrawError
micro_draw_from_ppm(const char *filename, MicroDrawCanvas **canvas);

#endif // MICRO_DRAW_PPM

//
// Implementation
//

#ifdef MICRO_DRAW_IMPLEMENTATION

#include <assert.h>

#ifndef MICRO_STATIC_ASSERT
#define MICRO_STATIC_ASSERT(cond, msg)                  \
  typedef char static_assertion_##msg##__LINE__[(cond) ? 1 : -1]
#endif
  
void micro_draw_canvas_init(MicroDrawCanvas *canvas,
                            int width, int height,
                            MicroDrawPixel pixel)
{
  if (!canvas) return;

  canvas->width  = width;
  canvas->height = height;
  canvas->pixel  = pixel;
  canvas->data   = MICRO_DRAW_MALLOC(width * height
                                     * micro_draw_get_channels(pixel));
}

void micro_draw_canvas_free(MicroDrawCanvas *canvas)
{
  MICRO_DRAW_FREE(canvas->data);
}  
  
MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_draw_get_channels);
MICRO_DRAW_DEF unsigned int micro_draw_get_channels(MicroDrawPixel pixel)
{
  switch(pixel)
  {
  case MICRO_DRAW_RGBA8:
    return 4;
  case MICRO_DRAW_BLACK_WHITE:
    return 1;
  default:
    break;
  }

  return 0;
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_draw_get_channel_size);
MICRO_DRAW_DEF unsigned int micro_draw_get_channel_size(MicroDrawPixel pixel)
{
  switch(pixel)
  {
  case MICRO_DRAW_RGBA8:
    return 1;
  case MICRO_DRAW_BLACK_WHITE:
    return 1;
  default:
    break;
  }

  return 0;
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_draw_color_to_rgba8);
MICRO_DRAW_DEF void
micro_draw_color_to_rgba8(unsigned char* color_src, MicroDrawPixel pixel_src,
                          unsigned char color_dest[4])
{
  assert(micro_draw_get_channels(MICRO_DRAW_RGBA8) == 4);
  assert(micro_draw_get_channel_size(MICRO_DRAW_RGBA8) == 1);
  
  switch(pixel_src)
  {
  case MICRO_DRAW_RGBA8:
    for (int i = 0; i < 4; ++i)
      color_dest[i] = color_src[i];
    break;
  case MICRO_DRAW_BLACK_WHITE:
    color_dest[0] = color_src[0] * 255;
    color_dest[1] = color_src[0] * 255;
    color_dest[2] = color_src[0] * 255;
    color_dest[3] = 255;
    break;
  default:
    break;
  }
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_draw_color_from_rgba8);
MICRO_DRAW_DEF void
micro_draw_color_from_rgba8(unsigned char color_src[4],
                            unsigned char *color_dest, MicroDrawPixel pixel_dest)
{
  assert(micro_draw_get_channels(MICRO_DRAW_RGBA8) == 4);
  assert(micro_draw_get_channel_size(MICRO_DRAW_RGBA8) == 1);

  switch(pixel_dest)
  {
  case MICRO_DRAW_RGBA8:
    for (int i = 0; i < 4; ++i)
      color_dest[i] = color_src[i];
    break;
  case MICRO_DRAW_BLACK_WHITE:
    color_dest[0] = color_src[0] == 255 ? 1 : 0;
    break;
  default:
    break;
  }
  return;
}

MICRO_DRAW_DEF void
micro_draw_color_convert(unsigned char *color_src, MicroDrawPixel pixel_src,
                         unsigned char *color_dest, MicroDrawPixel pixel_dest)
{
  assert(micro_draw_get_channels(MICRO_DRAW_RGBA8) == 4);
  assert(micro_draw_get_channel_size(MICRO_DRAW_RGBA8) == 1);

  unsigned char color_rgba[4] = {0};
  micro_draw_color_to_rgba8(color_src, pixel_src, color_rgba);
  micro_draw_color_from_rgba8(color_rgba, color_dest, pixel_dest);
  
  return;
}

static inline void *
_micro_draw_memcpy(void *dest, const void *src, unsigned int n)
{
  for (unsigned int i = 0; i < n; ++i)
    ((char*)dest)[i] = ((char*)src)[i];
  return dest;
}
  
MICRO_DRAW_DEF void
micro_draw_pixel(MicroDrawCanvas *canvas,
                 Vec2 pos,
                 unsigned char* color)
{
  if (!canvas || pos.x >= canvas->width || pos.x < 0
      || pos.y >= canvas->height || pos.y < 0) return;

  int channel_size = micro_draw_get_channel_size(canvas->pixel); // bytes
  unsigned int channels = micro_draw_get_channels(canvas->pixel);
  int index = (pos.y * canvas->width + pos.x) * channels * channel_size;
  
  _micro_draw_memcpy(&canvas->data[index], color, channels * channel_size);

  return;
}

MICRO_DRAW_DEF void
micro_draw_line(MicroDrawCanvas *canvas,
                Vec2 a, Vec2 b,
                unsigned char* color)
{
  if (!canvas) return;
  
  // Line equation
  double m = (a.y - b.y) / (double)(a.x - b.x);
  double q = a.y - m * a.x;
  int is_steep = (m > 1 || m < -1);

  if (is_steep)
  {
    // Transpose the image
    int tmp = a.x;
    a.x = a.y;
    a.y = tmp;

    tmp = b.x;
    b.x = b.y;
    b.y = tmp;

    m = (a.y - b.y) / (double)(a.x - b.x);
    q = a.y - m * a.x;
  }

  int start, end;
  if (a.x > b.x)
  {
    start = b.x;
    end = a.x;
  }
  else
  {
    start = a.x;
    end = b.x;
  }

  if (is_steep)
  {
    // Iterate by columns
    for (int p_x = start; p_x < end; ++p_x)
    {
      int p_y = m * p_x + q;
      // Retranspose the image
      micro_draw_pixel(canvas, (Vec2){ .x = p_y, .y = p_x }, color);
    }
  }
  else
  {
    // Transposed
    for (int p_x = start; p_x < end; ++p_x)
    {
      int p_y = m * p_x + q;
      micro_draw_pixel(canvas, (Vec2) { .x = p_x, .y = p_y }, color);
    }
  }
  
  return;
}

MICRO_DRAW_DEF void
micro_draw_clear(MicroDrawCanvas *canvas,
                 unsigned char *color)
{
  if (!canvas) return;
  
  for (int row = 0; row < canvas->height; ++row)
    for (int col = 0; col < canvas->width; ++col)
      micro_draw_pixel(canvas, (Vec2) { .x = col, .y = row }, color);
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    make_sure_that_color_dest_in_micro_draw_overlap_is_enough);
MICRO_DRAW_DEF void
micro_draw_overlap(MicroDrawCanvas *src_canvas,
                   MicroDrawCanvas *dest_canvas,
                   Vec2 offset)
{
  int channel_size = micro_draw_get_channel_size(src_canvas->pixel); // bytes
  unsigned int channels = micro_draw_get_channels(src_canvas->pixel);

  for (int row = 0; row < src_canvas->height; ++row)
  {
    for (int col = 0; col < src_canvas->width; ++col)
    {
      if (row < 0 || col < 0) continue;

      int index = (row * src_canvas->width + col) * channels * channel_size;
      unsigned char color_dest[4] = {0}; // 4 is enough for now
      micro_draw_color_convert(src_canvas->data + index, src_canvas->pixel,
                               color_dest, dest_canvas->pixel);
      micro_draw_pixel(dest_canvas,
                       (Vec2) { .x = col + offset.x, .y = row + offset.y },
                       color_dest);
    }
  }
  
  return;
}

MICRO_DRAW_DEF void
micro_draw_fill_rect(MicroDrawCanvas *canvas,
                     Rect rect,
                     unsigned char *color)
{

  micro_draw_fill_triangle(canvas,
                           (Triangle) {
                             .a_x = rect.a_x, .a_y = rect.a_y,
                             .b_x = rect.b_x, .b_y = rect.b_y,
                             .c_x = rect.c_x, .c_y = rect.c_y,
                           }, color);
  micro_draw_fill_triangle(canvas,
                           (Triangle) {
                             .a_x = rect.a_x, .a_y = rect.a_y,
                             .b_x = rect.c_x, .b_y = rect.c_y,
                             .c_x = rect.d_x, .c_y = rect.d_y,
                           }, color);

  return;
}

#define MICRO_DRAW_ABS(x) (((x) >= 0) ? (x) : -(x))

MICRO_DRAW_DEF void
micro_draw_fill_circle(MicroDrawCanvas *canvas,
                       Circle circle,
                       unsigned char *color)
{
  for (int row = circle.c_y - circle.r; row < circle.c_y + circle.r && row < canvas->height; ++row)
  {
    for (int col = circle.c_x - circle.r; col < circle.c_x + circle.r && col < canvas->width; ++col)
    {
      if (row < 0 || col < 0) continue;
      
      int dx = MICRO_DRAW_ABS(col - circle.c_x);
      int dy = MICRO_DRAW_ABS(row - circle.c_y);
      if (dx*dx + dy*dy > circle.r*circle.r) continue;
      
      micro_draw_pixel(canvas, (Vec2) { .x = col, .y = row }, color);
    }
  }

  return;
}

// Get the orientation of three 2D points (a, b, c).
//
// This computes the determinant:
//
// det( [[ a_x, a_y, 1 ],
//       [ b_x, b_y, 1 ],
//       [ c_x, c_y, 1 ]] )
//
// Algebraically, this is equal to
//
//   (b_x - a_x) * (c_y - a_y) - (b_y - a_y) * (c_x - a_x)
//
// which is twice the signed area of the triangle (a, b, c).
//
// The sign of the result indicates the orientation:
//   > 0 : counterclockwise
//   < 0 : clockwise
//   = 0 : collinear
//
// This orientation test is important for rasterization. Suppose we
// want to test whether a point P lies inside the triangle (a, b, c).
// We form the three sub-triangles (P, b, c), (a, P, c), and (a, b, P).
// If P is inside, all of these orientations will have the same sign
// as the orientation of the original triangle. If P is outside, at
// least one orientation will differ.
//
// See also: Fabian Giesen,
// “The barycentric conspiracies”
// https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
#define _micro_draw_orient2D(a_x, a_y, b_x, b_y, c_x, c_y) \
    ( ((b_x) - (a_x)) * ((c_y) - (a_y)) - ((b_y) - (a_y)) * ((c_x) - (a_x)) )

#define _micro_draw_min(a, b) ((a) < (b) ? (a) : (b))
#define _micro_draw_max(a, b) ((a) > (b) ? (a) : (b))
#define _micro_draw_min3(a, b, c) (_micro_draw_min(_micro_draw_min((a), (b)), (c)))
#define _micro_draw_max3(a, b, c) (_micro_draw_max(_micro_draw_max((a), (b)), (c)))

// https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
MICRO_DRAW_DEF void
micro_draw_fill_triangle(MicroDrawCanvas *canvas,
                         Triangle triangle,
                         unsigned char *color)
{
  // Compute triangle bounding box
  int minX = _micro_draw_min3(triangle.a_x, triangle.b_x, triangle.c_x);
  int minY = _micro_draw_min3(triangle.a_y, triangle.b_y, triangle.c_y);
  int maxX = _micro_draw_max3(triangle.a_x, triangle.b_x, triangle.c_x);
  int maxY = _micro_draw_max3(triangle.a_y, triangle.b_y, triangle.c_y);

  // Clip against screen bounds
  minX = _micro_draw_max(minX, 0);
  minY = _micro_draw_max(minY, 0);
  maxX = _micro_draw_min(maxX, canvas->width - 1);
  maxY = _micro_draw_min(maxY, canvas->height - 1);

  for (int row = minY; row <= maxY; ++row)
  {
    for (int col = minX; col <= maxX; ++col)
    {  
      // Determine barycentric coordinates
      int w0 = _micro_draw_orient2D(triangle.b_x, triangle.b_y, triangle.c_x, triangle.c_y, col, row);
      int w1 = _micro_draw_orient2D(triangle.c_x, triangle.c_y, triangle.a_x, triangle.a_y, col, row);
      int w2 = _micro_draw_orient2D(triangle.a_x, triangle.a_y, triangle.b_x, triangle.b_y, col, row);

      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0)
        micro_draw_pixel(canvas, (Vec2) { .x = col, .y = row }, color);
    }
  }

  return;
}
  
MICRO_DRAW_DEF void
micro_draw_grid(MicroDrawCanvas *canvas,
                int columns, int rows,
                unsigned char* color)
{
  if (!canvas) return;
  
  // Draw columns
  for (int x = 0; x < canvas->width; x += canvas->width / columns)
  {
    micro_draw_line(canvas,
                    (Vec2) { .x = x, .y = 0 },
                    (Vec2) { .x = x, .y = canvas->height},
                    color);
  }
  
  // Draw rows
  for (int y = 0; y < canvas->height; y += canvas->height / rows)
  {
    micro_draw_line(canvas,
                    (Vec2) { .x = 0, .y = y },
                    (Vec2) { .x = canvas->width, .y = y },
                    color);
  }
  return;
}

MICRO_DRAW_DEF void
micro_draw_get_color(MicroDrawCanvas *canvas,
                     Vec2 pos,
                     unsigned char** color)
{
  if (!canvas || pos.x >= canvas->width || pos.x < 0
      || pos.y >= canvas->height || pos.y < 0) return;

  int channel_size = micro_draw_get_channel_size(canvas->pixel); // bytes
  unsigned int channels = micro_draw_get_channels(canvas->pixel);
  int index = (pos.y * canvas->width + pos.x) * channels * channel_size;

  *color = canvas->data + index;
  return;
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    make_sure_that_color_dest_in_micro_draw_scaled_is_enough);
MICRO_DRAW_DEF void
micro_draw_scaled(MicroDrawCanvas *src_canvas,
                  MicroDrawCanvas *dest_canvas)
{
  for (int y = 0; y < dest_canvas->height; ++y)
  {
    for (int x = 0; x < dest_canvas->width; ++x)
    {
      int x_frame = (x * src_canvas->width) / (double)dest_canvas->width;
      int y_frame = (y * src_canvas->height) / (double)dest_canvas->height;
      unsigned char *color = NULL;
      micro_draw_get_color(src_canvas,
                           (Vec2) { .x = x_frame, .y = y_frame },
                           &color);

      unsigned char color_dest[4] = {0}; // 4 is enough for now
      micro_draw_color_convert(color, src_canvas->pixel,
                               color_dest, dest_canvas->pixel);
      micro_draw_pixel(dest_canvas,
                       (Vec2) { .x = x, .y = y },
                       color_dest);
    }
  }
  return;
}

static inline int _micro_draw_get_horizontal_characters(char* str)
{
  int max_num = 0;
  int num = 0;
  while (*str != '\0')
  {
    if (*str == '\n')
    {
      max_num = (num > max_num) ? num : max_num;
      num = 0;
    }
    else
    {
      num++;
    }
    str++;
  }
  return (num > max_num) ? num : max_num;
}

static inline int _micro_draw_get_vertical_characters(char* str)
{
  int num = 1;
  while (*str != '\0')
  {
    if (*str == '\n') num++;
    str++;
  }
  return num;
}

static inline int _micro_draw_strlen(char* str)
{
  int len = 0;
  while (str[len] != '\0') len++;
  return len;
}

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    make_sure_that_color_dest_in_micro_draw_text_is_enough);
MICRO_DRAW_DEF void
micro_draw_text(MicroDrawCanvas *canvas,
                MicroDrawText text,
                unsigned char* color)
{
  int channel_size = micro_draw_get_channel_size(canvas->pixel); // bytes
  unsigned int channels = micro_draw_get_channels(canvas->pixel);
  int char_x = MICRO_DRAW_CHARACTER_PIXELS_X * text.scale;
  int char_y = MICRO_DRAW_CHARACTER_PIXELS_Y * text.scale;
  int text_row = 0;
  int text_col = 0;
  int text_len = _micro_draw_strlen(text.text);
  
  for (int c = 0; c < text_len; ++c)
  {
    if (text.text[c] == '\n')
    {
      text_row++;
      text_col = 0;
      continue;
    }
    for (int y = 0; y < char_y; ++y)
    {
      for (int x = 0; x < char_x; ++x)
      {
        // Rescale the pixel
        int font_x = (x * MICRO_DRAW_FONT_WIDTH) / (double)char_x;
        int font_y = (y * MICRO_DRAW_FONT_HEIGHT) / (double)char_y;

        if (micro_draw_font[(int)text.text[c]][font_y][font_x] == 0)
          continue;
        
        // Calculate color
        unsigned char color_dest[4] = {0};
        for (unsigned int i = 0; i < channels * channel_size; ++i)
          color_dest[i] =
            color[i] *
            micro_draw_font[(int)text.text[c]][font_y][font_x];

        // TODO: implement transparency by multiplying color_dest by
        //       the color in data

        // Draw with translation
        micro_draw_pixel(canvas,
                         (Vec2) {
                           .x = x + text.x + text_col * char_x,
                           .y = y + text.y + text_row * char_y
                         },
                         color_dest);
      }
    }
    text_col++;
  }
  return;
}
  
#ifdef MICRO_DRAW_PPM

#include <stdio.h>  // fread
#include <stdlib.h> // atoi

// The PPM header starts with 4 values speareted by either space or
// newline: ID, WIDTH, HEIGHT, MAX_COLOR_VALUE.
// Where ID is either:
//  - P1: ASCII Bitmap   (.pbm)
//  - P2: ASCII Graymap  (.pgm)
//  - P3: ASCII PixMap   (.ppm)
//  - P4: Binary Bitmap  (.pbm)
//  - P5: Binary Graymap (.pgm)
//  - P6: Binary PixMap  (.ppm)
//
// The rest of the file contains WIDHT*HEIGHT color values less then
// MAX_COLOR_VALUE.
MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_draw_to_ppm);
MICRO_DRAW_DEF MicroDrawError
micro_draw_to_ppm(const char *filename, MicrDrawCanvas *canvas)
{
  MicroDrawError error = MICRO_DRAW_OK;
  
  FILE* file = fopen(filename, "w+");
  if (file == NULL)
  {
    perror("Error opening file");
    return MICRO_DRAW_ERROR_OPEN_FILE;
  }

  int channels = micro_draw_get_channels(canvas->pixel);
  int channel_size = micro_draw_get_channel_size(canvas->pixel);
  int data_size = canvas->width * canvas->height * channels * channel_size;

  switch(pixel)
  {
  case MICRO_DRAW_RGBA8:
    
    fprintf(file, "P6\n%d %d\n%d\n", canvas->width,
            canvas->height, (1 << (channel_size * 8)) - 1);
    for (int i = 0; i < data_size; i += channels * channel_size)
    {
      fwrite(canvas->data + i, channel_size, (channels - 1), file);
    }
    break;
    
  case MICRO_DRAW_BLACK_WHITE:
    
    fprintf(file, "P1\n%d %d\n%d\n", canvas->width, canvas->height, 1);
    for (int i = 0; i < data_size; i += channels * channel_size)
    {
      fwrite(canvas->data + i, channel_size, (channels - 1), file);
    }
    break;
    
  default:
    goto done;
  }

 done:
  fclose(file);
  return error;
}

static inline int _micro_draw_strcmp(const char *s1, const char *s2)
{
  while (*s1 == *s2) {
    s1++;
    s2++;
  }
  if (*s2 == '\0')
    return 0;
  return 1;
}

static inline int _micro_draw_is_whitespace(char c)
{
  return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

typedef enum {
  _MICRO_DRAW_P1 = 0,
  _MICRO_DRAW_P2,
  _MICRO_DRAW_P3,
  _MICRO_DRAW_P4,
  _MICRO_DRAW_P5,
  _MICRO_DRAW_P6,
} _MicroDrawPPMType;

MICRO_STATIC_ASSERT(_MICRO_DRAW_PIXEL_MAX == 2,
                    should_also_update_micro_from_to_ppm);
MICRO_DRAW_DEF MicroDrawError
micro_draw_from_ppm(const char* filename, MicroDrawCanvas **canvas)
{
  _MicroDrawPPMType file_type = 0;
  int color_max = 0;
  MicroDrawError error = MICRO_DRAW_OK;
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    perror("Error opening file");
    return MICRO_DRAW_ERROR_OPEN_FILE;
  }

  // Parse header ----------------------------------------------------
  
  char magic[3];
  fread(magic, 1, 3, file);
  if (_micro_draw_strcmp(magic, "P1") == 0)
  {
    file_type = _MICRO_DRAW_P1;
  } else if (_micro_draw_strcmp(magic, "P2") == 0)
  {
    file_type = _MICRO_DRAW_P2;
  } else if (_micro_draw_strcmp(magic, "P3") == 0)
  {
    file_type = _MICRO_DRAW_P3;
  } else if (_micro_draw_strcmp(magic, "P4") == 0)
  {
    file_type = _MICRO_DRAW_P4;
  } else if (_micro_draw_strcmp(magic, "P5") == 0)
  {
    file_type = _MICRO_DRAW_P5;
  } else if (_micro_draw_strcmp(magic, "P6") == 0)
  {
    file_type = _MICRO_DRAW_P6;
  }
  else {
    error = MICRO_DRAW_ERROR_INVALID_MAGIC_NUMBER;
    goto done;
  }

  // Data width
  {
    char num_buff[30] = {0};
    int position = 0;
    do
    {
      fread(num_buff + position, 1, 1, file);
      position++;
    } while (!_micro_draw_is_whitespace(num_buff[position-1]) && position < 30);
    canvas->width = atoi(num_buff);
    if (canvas->width == 0)
    {
      error = MICRO_DRAW_ERROR_INVALID_IMAGE_SIZE;
      goto done;
    }
  }

  // Data height
  {
    char num_buff[30] = {0};
    int position = 0;
    do
    {
      fread(num_buff + position, 1, 1, file);
      position++;
    } while (!_micro_draw_is_whitespace(num_buff[position-1]) && position < 30);
    canvas->height = atoi(num_buff);
    if (canvas->height == 0)
    {
      error = MICRO_DRAW_ERROR_INVALID_IMAGE_SIZE;
      goto done;
    }
  }

  // Color max
  if (file_type != _MICRO_DRAW_P1 && file_type != _MICRO_DRAW_P4)
  {
    char num_buff[30] = {0};
    int position = 0;
    do
    {
      fread(num_buff + position, 1, 1, file);
      position++;
    } while (!_micro_draw_is_whitespace(num_buff[position-1]) && position < 30);
    color_max = atoi(num_buff);
    if (color_max == 0)
    {
      error = MICRO_DRAW_ERROR_INVALID_IMAGE_SIZE;
      goto done;
    }
  }

  // Parse raster ----------------------------------------------------
  
  // TODO
  (void) data;
  (void) data_width;
  (void) data_height;
  (void) pixel;
  (void) color_max;
  (void) file_type;
  
  assert(0 && "Not implemented");
  
 done:
  fclose(file);
  return error;
}

#endif // MICRO_DRAW_PPM

unsigned char
micro_draw_font[128][MICRO_DRAW_FONT_HEIGHT][MICRO_DRAW_FONT_WIDTH] = {
  ['a'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},    
  },
  ['b'] = {
    {1, 0, 0, 0, 0},
    {1, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 0, 0},
  },
  ['c'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['d'] = {
    {0, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
  },
  ['e'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
  },
  ['f'] = {
    {0, 0, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
  },
  ['g'] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['h'] = {
    {1, 0, 0, 0, 0},
    {1, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
  },
  ['i'] = {
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
  },
  ['j'] = {
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {1, 0, 1, 0, 0},
    {0, 1, 1, 0, 0},
  },
  ['k'] = {
    {1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
  },
  ['l'] = {
    {0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0},
  },
  ['m'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 0, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
  },
  ['n'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
  },
  ['o'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['p'] = {
    {1, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
  },
  ['q'] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0},
  },
  ['r'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 1, 1, 0},
    {1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
  },
  ['s'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {1, 1, 1, 0, 0},
  },
  ['t'] = {
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['u'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
  },
  ['v'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['w'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {0, 1, 1, 1, 1},
  },
  ['x'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 1, 0, 0},
  },
  ['y'] = {
    {0, 0, 0, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
  },
  ['z'] = {
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
  },
['A'] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
  },
  ['B'] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
  },
  ['C'] = {
    {0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1},
  },
  ['D'] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
  },
  ['E'] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
  },
  ['F'] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
  },
  ['G'] = {
    {0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 1},
  },
  ['H'] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
  },
  ['I'] = {
    {0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0},
  },
  ['J'] = {
    {0, 0, 1, 1, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['K'] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1},
  },
  ['L'] = {
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
  },
  ['M'] = {
    {1, 0, 0, 0, 1},
    {1, 1, 0, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
  },
  ['N'] = {
    {1, 0, 0, 0, 1},
    {1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
  },
  ['O'] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
  },
  ['P'] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
  },
  ['Q'] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
  },
  ['R'] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1},
  },
  ['S'] = {
    {0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
  },
  ['T'] = {
    {1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
  },
  ['U'] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
  },
  ['V'] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
  },
  ['W'] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 1, 0, 1, 1},
    {1, 0, 0, 0, 1},
  },
  ['X'] = {
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1},
  },
  ['Y'] = {
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
  },
  ['Z'] = {
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
  },
  ['0'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['1'] = {
    {0, 0, 1, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0},
  },
  ['2'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
  },
  ['3'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['4'] = {
    {0, 0, 1, 1, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 0},
  },
  ['5'] = {
    {1, 1, 1, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 0, 0},
    {0, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['6'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['7'] = {
    {1, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
  },
  ['8'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  ['9'] = {
    {0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0},
  },
  [','] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
  },
  ['!'] = {
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0},
  },
  ['.'] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
  },
  ['-'] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
  },
  [':'] = {
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0},
  },
};

#endif // MICRO_DRAW_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // MICRO_DRAW
