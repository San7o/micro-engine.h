// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o
//
// micro-elements.h
// ================
//
//  A point is that of which there is no part.
//  And a line is a length without breadth.
//  And the extremities of a line are points.
//  A straight-line is (any) one which lies evenly with points on itself.
//  And a surface is that which has length and breadth
//
//                               - Elements, Book 1, Euclid
//

#ifndef MICRO_ELEMENTS_H
#define MICRO_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vertex {
  int x;
  int y;
  int z;
  
  int u;
  int v;
} Vertex;

typedef struct Mesh {
  Vertex       *vertexes;
  unsigned int  size;
} Mesh;

typedef struct Sprite {
  unsigned char *data;
  unsigned int   width;
  unsigned int   height;
  unsigned int   channels;
} Texture;

typedef struct Altas {
  unsigned char *data;
  unsigned int   width;
  unsigned int   height;
  unsigned int   channels;
  unsigned int   sprite_x;
  unsigned int   sprite_y;
} Texture;
  
typedef struct Layer {
  unsigned char *data;
  unsigned int   width;
  unsigned int   height;
  unsigned int   channels;
  unsigned int   z;
} Layer;

typedef struct Scene {
  Layer        *layers;
  unsigned int  num_layers;
} Scene;

#ifdef __cplusplus
}
#endif
  
#endif // MICRO_ELEMENTS_H
