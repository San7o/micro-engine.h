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

//
// Function declarations
//

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
micro_draw_color_from_rgba(unsigned char* color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_get_color(unsigned char* data, int data_width, int data_height,
                     int x, int y, unsigned char** color, MicroDrawPixel pixel);

// Primitives --------------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_pixel(unsigned char* data, int data_width, int data_height,
                 int x, int y, unsigned char* color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_line(unsigned char* data, int data_width, int data_height,
                int a_x, int a_y,int b_x, int b_y,
                unsigned char* color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_fill_rect(unsigned char* data, int data_width, int data_height,
                     int x, int y, int w, int h,
                     unsigned char *color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_fill_circle(unsigned char* data, int data_width, int data_height,
                       int center_x, int center_y, int radius,
                       unsigned char *color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_fill_triangle(unsigned char *data, int data_width, int data_height,
                         int a_x, int a_y, int b_x, int b_y, int c_x, int c_y,
                         unsigned char *color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_grid(unsigned char* data, int data_width, int data_height,
                int columns, int rows, unsigned char* color, MicroDrawPixel pixel);

MICRO_DRAW_DEF void
micro_draw_clear(unsigned char* data, int data_width, int data_height,
                 unsigned char *color, MicroDrawPixel pixel);

// Transformations ---------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_scaled(unsigned char* source_data,
                  int source_data_width, int souce_data_height,
                  MicroDrawPixel src_pixel, unsigned char* dest_data,
                  int dest_data_width, int dest_data_height,
                  MicroDrawPixel dest_pixel);

MICRO_DRAW_DEF void
micro_draw_overlap(unsigned char* src_data, int src_data_width,
                   int src_data_height, MicroDrawPixel src_pixel,
                   unsigned char* dest_data, int dest_data_width,
                   int dest_data_height, MicroDrawPixel dest_pixel,
                   int x_offset, int y_offset);
  
// Text --------------------------------------------------------------
  
MICRO_DRAW_DEF void
micro_draw_text(unsigned char* data, int data_width, int data_height,
                MicroDrawPixel pixel_data, char* text, int text_x,
                int text_y, float text_scale, unsigned char* text_color);

// PPM ---------------------------------------------------------------
  
#ifdef MICRO_DRAW_PPM

MICRO_DRAW_DEF MicroDrawError
micro_draw_to_ppm(const char *filename, unsigned char *data,
                  int data_width, int data_height, MicroDrawPixel pixel);


MICRO_DRAW_DEF MicroDrawError
micro_draw_from_ppm(const char *filename, unsigned char **data,
                    int *data_width, int *data_height, MicroDrawPixel *pixel);

#endif // MICRO_DRAW_PPM

//
// Implementation
//

#ifdef MICRO_DRAW_IMPLEMENTATION

#include <assert.h>

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_draw_get_channels");
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

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_draw_get_channel_size");
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

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_draw_color_to_rgba8");
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

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_draw_color_from_rgba8");
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
micro_draw_pixel(unsigned char* data, int data_width, int data_height,
                 int x, int y, unsigned char* color, MicroDrawPixel pixel)
{
  if (x >= data_width || x < 0 || y >= data_height || y < 0) return;

  int channel_size = micro_draw_get_channel_size(pixel); // bytes
  unsigned int channels = micro_draw_get_channels(pixel);
  int index = (y * data_width + x) * channels * channel_size;
  
  _micro_draw_memcpy(&data[index], color, channels * channel_size);

  return;
}

MICRO_DRAW_DEF void
micro_draw_line(unsigned char* data, int data_width, int data_height,
                int a_x, int a_y, int b_x, int b_y,
                unsigned char* color, MicroDrawPixel pixel)
{
  // Line equation
  double m = (a_y - b_y) / (double)(a_x - b_x);
  double q = a_y - m * a_x;
  int is_steep = (m > 1 || m < -1);

  if (is_steep)
  {
    // Transpose the image
    int tmp = a_x;
    a_x = a_y;
    a_y = tmp;

    tmp = b_x;
    b_x = b_y;
    b_y = tmp;

    m = (a_y - b_y) / (double)(a_x - b_x);
    q = a_y - m * a_x;
  }

  int start, end;
  if (a_x > b_x)
  {
    start = b_x;
    end = a_x;
  }
  else
  {
    start = a_x;
    end = b_x;
  }

  if (is_steep)
  {
    // Iterate by columns
    for (int p_x = start; p_x < end; ++p_x)
    {
      int p_y = m * p_x + q;
      // Retranspose the image
      micro_draw_pixel(data, data_width, data_height,
                       p_y, p_x, color, pixel);
    }
  }
  else
  {
    // Transposed
    for (int p_x = start; p_x < end; ++p_x)
    {
      int p_y = m * p_x + q;
      micro_draw_pixel(data, data_width, data_height,
                       p_x, p_y, color, pixel);
    }
  }
  
  return;
}

MICRO_DRAW_DEF void
micro_draw_clear(unsigned char* data, int data_width, int data_height,
                 unsigned char *color, MicroDrawPixel pixel)
{
  for (int row = 0; row < data_height; ++row)
    for (int col = 0; col < data_width; ++col)
      micro_draw_pixel(data, data_width, data_height,
                       col, row, color, pixel);
}

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "MicroDrawPixel has changed, make sure that color_dest in micro_draw_overlap is enough");
MICRO_DRAW_DEF void
micro_draw_overlap(unsigned char* src_data, int src_data_width,
                   int src_data_height, MicroDrawPixel src_pixel,
                   unsigned char* dest_data, int dest_data_width,
                   int dest_data_height, MicroDrawPixel dest_pixel,
                   int x_offset, int y_offset)
{
  int channel_size = micro_draw_get_channel_size(src_pixel); // bytes
  unsigned int channels = micro_draw_get_channels(src_pixel);

  for (int row = 0; row < src_data_height; ++row)
  {
    for (int col = 0; col < src_data_width; ++col)
    {
      if (row < 0 || col < 0) continue;

      int index = (row * src_data_width + col) * channels * channel_size;
      unsigned char color_dest[4] = {0}; // 4 is enough for now
      micro_draw_color_convert(src_data + index, src_pixel,
                               color_dest, dest_pixel);
      micro_draw_pixel(dest_data, dest_data_width, dest_data_height,
                       col + x_offset, row + y_offset, color_dest, dest_pixel);
    }
  }
  
  return;
}

MICRO_DRAW_DEF void
micro_draw_fill_rect(unsigned char* data, int data_width, int data_height,
                     int x, int y, int w, int h, unsigned char *color,
                     MicroDrawPixel pixel)
{
  for (int row = y; row < h + y && row < data_height; ++row)
  {
    for (int col = x; col < w + x && col < data_width; ++col)
    {
      if (row < 0 || col < 0) continue;
      micro_draw_pixel(data, data_width, data_height,
                       col, row, color, pixel);
    }
  }
  
  return;
}

#define MICRO_DRAW_ABS(x) (((x) >= 0) ? (x) : -(x))

MICRO_DRAW_DEF void
micro_draw_fill_circle(unsigned char* data, int data_width, int data_height,
                       int center_x, int center_y, int radius,
                       unsigned char *color, MicroDrawPixel pixel)
{
  for (int row = center_y - radius; row < center_y + radius && row < data_height; ++row)
  {
    for (int col = center_x - radius; col < center_x + radius && col < data_width; ++col)
    {
      if (row < 0 || col < 0) continue;
      
      int dx = MICRO_DRAW_ABS(col - center_x);
      int dy = MICRO_DRAW_ABS(row - center_y);
      if (dx*dx + dy*dy > radius*radius) continue;
      
      micro_draw_pixel(data, data_width, data_height,
                       col, row, color, pixel);
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
micro_draw_fill_triangle(unsigned char *data, int data_width, int data_height,
                         int a_x, int a_y, int b_x, int b_y,
                         int c_x, int c_y, unsigned char *color,
                         MicroDrawPixel pixel)
{
  // Compute triangle bounding box
  int minX = _micro_draw_min3(a_x, b_x, c_x);
  int minY = _micro_draw_min3(a_y, b_y, c_y);
  int maxX = _micro_draw_max3(a_x, b_x, c_x);
  int maxY = _micro_draw_max3(a_y, b_y, c_y);

  // Clip against screen bounds
  minX = _micro_draw_max(minX, 0);
  minY = _micro_draw_max(minY, 0);
  maxX = _micro_draw_min(maxX, data_width - 1);
  maxY = _micro_draw_min(maxY, data_height - 1);

  for (int row = minY; row <= maxY; ++row)
  {
    for (int col = minX; col <= maxX; ++col)
    {  
      // Determine barycentric coordinates
      int w0 = _micro_draw_orient2D(b_x, b_y, c_x, c_y, col, row);
      int w1 = _micro_draw_orient2D(c_x, c_y, a_x, a_y, col, row);
      int w2 = _micro_draw_orient2D(a_x, a_y, b_x, b_y, col, row);

      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0)
        micro_draw_pixel(data, data_width, data_height,
                         col, row, color, pixel);
    }
  }

  return;
}
  
MICRO_DRAW_DEF void
micro_draw_grid(unsigned char* data, int data_width, int data_height,
                int columns, int rows, unsigned char* color, MicroDrawPixel pixel)
{
  // Draw columns
  for (int x = 0; x < data_width; x += data_width / columns)
  {
    micro_draw_line(data, data_width, data_height,
                     x, 0, x, data_height,
                     color, pixel);
  }
  
  // Draw rows
  for (int y = 0; y < data_height; y += data_height / rows)
  {
    micro_draw_line(data, data_width, data_height,
                     0, y, data_width, y,
                     color, pixel);
  }
  return;
}

MICRO_DRAW_DEF void
micro_draw_get_color(unsigned char* data, int data_width, int data_height,
                     int x, int y, unsigned char** color, MicroDrawPixel pixel)
{
  if (x >= data_width || x < 0 || y >= data_height || y < 0) return;

  int channel_size = micro_draw_get_channel_size(pixel); // bytes
  unsigned int channels = micro_draw_get_channels(pixel);
  int index = (y * data_width + x) * channels * channel_size;

  *color = data + index;
  return;
}

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "MicroDrawPixel has changed, make sure that color_dest in micro_draw_scaled is enough");
MICRO_DRAW_DEF void
micro_draw_scaled(unsigned char* src_data, int src_data_width, int src_data_height,
                  MicroDrawPixel src_pixel, unsigned char* dest_data,
                  int dest_data_width, int dest_data_height,
                  MicroDrawPixel dest_pixel)
{
  for (int y = 0; y < dest_data_height; ++y)
  {
    for (int x = 0; x < dest_data_width; ++x)
    {
      int x_frame = (x * src_data_width) / (double)dest_data_width;
      int y_frame = (y * src_data_height) / (double)dest_data_height;
      unsigned char *color = NULL;
      micro_draw_get_color(src_data, src_data_width, src_data_height,
                           x_frame, y_frame, &color, src_pixel);

      unsigned char color_dest[4] = {0}; // 4 is enough for now
      micro_draw_color_convert(color, src_pixel, color_dest, dest_pixel);
      micro_draw_pixel(dest_data, dest_data_width, dest_data_height,
                       x, y, color_dest, dest_pixel);
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

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "MicroDrawPixel has changed, make sure that color_dest in micro_draw_text is enough");
MICRO_DRAW_DEF void
micro_draw_text(unsigned char* data, int data_width, int data_height,
                MicroDrawPixel pixel_data, char* text, int text_x,
                int text_y, float text_scale, unsigned char* text_color)
{
  int channel_size = micro_draw_get_channel_size(pixel_data); // bytes
  unsigned int channels = micro_draw_get_channels(pixel_data);
  int char_x = MICRO_DRAW_CHARACTER_PIXELS_X * text_scale;
  int char_y = MICRO_DRAW_CHARACTER_PIXELS_Y * text_scale;
  int text_row = 0;
  int text_col = 0;
  int text_len = _micro_draw_strlen(text);
  
  for (int c = 0; c < text_len; ++c)
  {
    if (text[c] == '\n')
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

        if (micro_draw_font[(int)text[c]][font_y][font_x] == 0)
          continue;
        
        // Calculate color
        unsigned char color_dest[4] = {0};
        for (unsigned int i = 0; i < channels * channel_size; ++i)
          color_dest[i] =
            text_color[i] *
            micro_draw_font[(int)text[c]][font_y][font_x];

        // TODO: implement transparency by multiplying color_dest by
        //       the color in data

        // Draw with translation
        micro_draw_pixel(data, data_width, data_height,
                         x + text_x + text_col * char_x,
                         y + text_y + text_row * char_y, color_dest, pixel_data);
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
_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_draw_to_ppm");
MICRO_DRAW_DEF MicroDrawError
micro_draw_to_ppm(const char *filename, unsigned char *data,
                  int data_width, int data_height, MicroDrawPixel pixel)
{
  MicroDrawError error = MICRO_DRAW_OK;
  
  FILE* file = fopen(filename, "w+");
  if (file == NULL)
  {
    perror("Error opening file");
    return MICRO_DRAW_ERROR_OPEN_FILE;
  }

  int channels = micro_draw_get_channels(pixel);
  int channel_size = micro_draw_get_channel_size(pixel);
  int data_size = data_width * data_height * channels * channel_size;

  switch(pixel)
  {
  case MICRO_DRAW_RGBA8:
    
    fprintf(file, "P6\n%d %d\n%d\n", data_width,
            data_height, (1 << (channel_size * 8)) - 1);
    for (int i = 0; i < data_size; i += channels * channel_size)
    {
      fwrite(data + i, channel_size, (channels - 1), file);
    }
    break;
    
  case MICRO_DRAW_BLACK_WHITE:
    
    fprintf(file, "P1\n%d %d\n%d\n", data_width, data_height, 1);
    for (int i = 0; i < data_size; i += channels * channel_size)
    {
      fwrite(data + i, channel_size, (channels - 1), file);
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

_Static_assert(_MICRO_DRAW_PIXEL_MAX == 2,
               "Updated MicroDrawPixel, should also update micro_from_to_ppm");
MICRO_DRAW_DEF MicroDrawError
micro_draw_from_ppm(const char* filename, unsigned char **data,
                    int *data_width, int *data_height, MicroDrawPixel *pixel)
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
    *data_width = atoi(num_buff);
    if (*data_width == 0)
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
    *data_height = atoi(num_buff);
    if (*data_width == 0)
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
    if (*data_width == 0)
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
  ['A'] = {{0}},
  ['B'] = {{0}},
  ['C'] = {{0}},
  ['D'] = {{0}},
  ['E'] = {{0}},
  ['F'] = {{0}},
  ['G'] = {{0}},
  ['H'] = {{0}},
  ['I'] = {{0}},
  ['J'] = {{0}},
  ['K'] = {{0}},
  ['L'] = {{0}},
  ['M'] = {{0}},
  ['N'] = {{0}},
  ['O'] = {{0}},
  ['P'] = {{0}},
  ['Q'] = {{0}},
  ['R'] = {{0}},
  ['S'] = {{0}},
  ['T'] = {{0}},
  ['U'] = {{0}},
  ['V'] = {{0}},
  ['W'] = {{0}},
  ['X'] = {{0}},
  ['Y'] = {{0}},
  ['Z'] = {{0}},
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
};

#endif // MICRO_DRAW_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // MICRO_DRAW
