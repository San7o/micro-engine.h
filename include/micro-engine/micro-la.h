///////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-la.h
// ==========
//
// Linear algebra types and functions as an header-only library with a
// C99 and C11 api.
//
// THIS LIBRARY IS NOT FINISHED
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o
//
//
// Features
// --------
//
// The library provides generic definitions and transformations for
// the types:
//
//   - Vec2
//   - Vec3
//   - Vec4
//   - Quaternion
//   - Rect
//   - Circle
//   - Triangle
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_LA_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_LA_IMPLEMENTATION
//   #include "micro-la.h"
//
// You can tune the library by #defining certain values. See the
// "Config" comments under "Configuration" below.
//
//
// Code
// ----
//
// The official git repository of micro-la.h is hosted at:
//
//     https://github.com/San7o/micro-la.h
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
//  - TODO: implement quaternion rotations for any axes
//  - TODO: add rotations to shapes Rect and Triangle
//  - TODO: add vector operations like add, sub, dot, eq, ...
//  - TODO: add generic poligons with any number of vertices
//

#ifndef MICRO_LA
#define MICRO_LA

#define MICRO_LA_MAJOR 0
#define MICRO_LA_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Configuration
//

// Config: Prefix for all functions
// For function inlining, set this to `static inline` and then define
// the implementation in all the files
#ifndef MICRO_LA_DEF
  #define MICRO_LA_DEF static inline
#endif

//
// Macros
//

//
// Errors
//

#define MICRO_LA_OK          0
#define _MICRO_LA_ERROR_MAX -1

//
// Types
//

#if __STDC_VERSION__ >= 201112L // >= C11

typedef union {
  struct { double a; double b; double c; double d; };
  struct { double r; double i; double j; double k; };
  struct { double first; double second; double third; double fourth; };
  double v[4];
} Quaternion;

#else

typedef struct {
  double a;
  double b;
  double c;
  double d;
} Quaternion;

#endif
    
MICRO_LA_DEF Quaternion micro_la_quaternion_prod(Quaternion p, Quaternion q);
MICRO_LA_DEF Quaternion micro_la_quaternion_recip(Quaternion p);
MICRO_LA_DEF Quaternion micro_la_quaternion_rotate(Quaternion p, double angle);

#include <math.h>

#define Vec2_IMPL(type, suffix)                             \
  MICRO_LA_DEF Vec2##suffix                                 \
  Vec2##suffix##_scale(Vec2##suffix vec,                    \
                       Vec2##suffix scale)                  \
  {                                                         \
    return (Vec2##suffix) {                                 \
      .x = vec.x * scale.x, .y = vec.y * scale.y,           \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec2##suffix                                 \
  Vec2##suffix##_translate(Vec2##suffix vec,                \
                           Vec2##suffix translation)        \
  {                                                         \
    return (Vec2##suffix) {                                 \
      .x = vec.x + translation.x,                           \
      .y = vec.y + translation.y,                           \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec2##suffix                                 \
  Vec2##suffix##_rotate(Vec2##suffix vec,                   \
                        double angle)                       \
  {                                                         \
    Quaternion vec_quaternion =                             \
      (Quaternion){ .a = 0, .b = vec.x, .c = vec.y, .d = 0};\
    Quaternion rotated_quaternion =                         \
      micro_la_quaternion_rotate(vec_quaternion, angle);    \
    return (Vec2##suffix) {                                 \
      .x = rotated_quaternion.b, .y = rotated_quaternion.c, \
    };                                                      \
  }

#define Vec3_IMPL(type, suffix)                             \
  MICRO_LA_DEF Vec3##suffix                                 \
  Vec3##suffix##_scale(Vec3##suffix vec,                    \
                     Vec3##suffix scale)                    \
  {                                                         \
    return (Vec3##suffix) {                                 \
      .x = vec.x * scale.x, .y = vec.y * scale.y,           \
      .z = vec.z * scale.z,                                 \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec3##suffix                                 \
  Vec3##suffix##_translate(Vec3##suffix vec,                \
                           Vec3##suffix translation)        \
  {                                                         \
    return (Vec3##suffix) {                                 \
      .x = vec.x + translation.x,                           \
      .y = vec.y + translation.y,                           \
      .z = vec.z + translation.z,                           \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec3##suffix                                 \
  Vec3##suffix##_rotate(Vec3##suffix vec,                   \
                        double angle)                       \
  {                                                         \
    Quaternion vec_quaternion =                             \
      (Quaternion){ .a = 0, .b = vec.x, .c = vec.y, .d = vec.z};\
    Quaternion rotated_quaternion =                         \
      micro_la_quaternion_rotate(vec_quaternion, angle);    \
    return (Vec3##suffix) {                                 \
      .x = rotated_quaternion.b, .y = rotated_quaternion.c, \
      .z = rotated_quaternion.d,                            \
    };                                                      \
  }

#define Vec4_IMPL(type, suffix)                             \
  MICRO_LA_DEF Vec4##suffix                                 \
  Vec4##suffix##_scale(Vec4##suffix vec,                    \
                       Vec4##suffix scale)                  \
  {                                                         \
    return (Vec4##suffix) {                                 \
      .a = vec.a * scale.a, .b = vec.b * scale.b,           \
      .c = vec.c * scale.c, .d = vec.d * scale.d,           \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec4##suffix                                 \
  Vec4##suffix##_translate(Vec4##suffix vec,                \
                           Vec4##suffix translation)        \
  {                                                         \
    return (Vec4##suffix) {                                 \
      .a = vec.a + translation.a,                           \
      .b = vec.b + translation.b,                           \
      .c = vec.c + translation.c,                           \
      .d = vec.d + translation.d,                           \
    };                                                      \
  }

#define Rect_IMPL(type, suffix)                             \
  MICRO_LA_DEF Rect##suffix                                 \
  Rect##suffix##_scale(Rect##suffix rect,                   \
                       Vec2f scale)                         \
  {                                                         \
    return (Rect##suffix) {                                 \
      .a_x = rect.a_x * scale.x, .a_y = rect.a_y * scale.y, \
      .b_x = rect.b_x * scale.x, .b_y = rect.b_y * scale.y, \
      .c_x = rect.c_x * scale.x, .c_y = rect.c_y * scale.y, \
      .d_x = rect.d_x * scale.x, .d_y = rect.d_y * scale.y, \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Rect##suffix                                 \
  Rect##suffix##_translate(Rect##suffix rect,               \
                           Vec2f translation)               \
  {                                                         \
    return (Rect##suffix) {                                 \
      .a_x = rect.a_x * translation.x,                      \
      .a_y = rect.a_y * translation.y,                      \
      .b_x = rect.b_x * translation.x,                      \
      .b_y = rect.b_y * translation.y,                      \
      .c_x = rect.c_x * translation.x,                      \
      .c_y = rect.c_y * translation.y,                      \
      .d_x = rect.d_x * translation.x,                      \
      .d_y = rect.d_y * translation.y,                      \
    };                                                      \
  }

#define Circle_IMPL(type, suffix)                           \
  MICRO_LA_DEF Circle##suffix                               \
  Circle##suffix##_scale(Circle##suffix circle,             \
                         float scale)                       \
  {                                                         \
    return (Circle##suffix) {                               \
      .c_x = circle.c_x, .c_y = circle.c_y,                 \
      .r = circle.r * scale,                                \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Circle##suffix                               \
  Circle##suffix##_translate(Circle##suffix circle,         \
                             Vec2f translation)             \
  {                                                         \
    return (Circle##suffix) {                               \
      .c_x = circle.c_x + translation.x,                    \
      .c_y = circle.c_y + translation.y,                    \
      .r = circle.r,                                        \
    };                                                      \
  }
  
#define Triangle_IMPL(type, suffix)                         \
  MICRO_LA_DEF Triangle##suffix                             \
  Triangle##suffix##_scale(Triangle##suffix triangle,       \
                       Vec2f scale)                         \
  {                                                         \
    return (Triangle##suffix) {                             \
      .a_x = triangle.a_x * scale.x,                        \
      .a_y = triangle.a_y * scale.y,                        \
      .b_x = triangle.b_x * scale.x,                        \
      .b_y = triangle.b_y * scale.y,                        \
      .c_x = triangle.c_x * scale.x,                        \
      .c_y = triangle.c_y * scale.y,                        \
    };                                                      \
  }                                                         \
                                                            \
  MICRO_LA_DEF Triangle##suffix                             \
  Triangle##suffix##_translate(Triangle##suffix triangle,   \
                           Vec2f translation)               \
  {                                                         \
    return (Triangle##suffix) {                             \
      .a_x = triangle.a_x * translation.x,                  \
      .a_y = triangle.a_y * translation.y,                  \
      .b_x = triangle.b_x * translation.x,                  \
      .b_y = triangle.b_y * translation.y,                  \
      .c_x = triangle.c_x * translation.x,                  \
      .c_y = triangle.c_y * translation.y,                  \
    };                                                      \
  }

  
// C11 onwards supports unnamed struct and unions
#if __STDC_VERSION__ >= 201112L // >= C11

#define Vec2_DEF(type, suffix)                  \
  typedef union {                               \
    struct { type x; type y; };                 \
    struct { type a; type b; };                 \
    struct { type first; type second; };        \
    type v[2];                                  \
  } Vec2##suffix;                               \
  Vec2_IMPL(type, suffix)

#define Vec3_DEF(type, suffix)                        \
  typedef union {                                     \
    struct { type x; type y; type z; };               \
    struct { type a; type b; type c; };               \
    struct { type first; type second; type third; };  \
    type v[3];                                        \
  } Vec3##suffix;                                     \
  Vec3_IMPL(type, suffix)

#define Vec4_DEF(type, suffix)                                    \
  typedef union {                                                 \
    struct { type a; type b; type c; type d; };                   \
    struct { type r; type i; type j; type k; };                   \
    struct { type first; type second; type third; type fourth; }; \
    type v[4];                                                    \
  } Vec4##suffix;                                                 \
  Vec4_IMPL(type, suffix)

#define Rect_DEF(type, suffix)                  \
  typedef union {                               \
    struct {                                    \
      type a_x; type a_y;                       \
      type b_x; type b_y;                       \
      type c_x; type c_y;                       \
      type d_x; type d_y;                       \
    };                                          \
    type v[8];                                  \
  } Rect##suffix;                               \
  Rect_IMPL(type, suffix)

#define Circle_DEF(type, suffix)                \
  typedef union {                               \
    struct { type c_x; type c_y; type r; };     \
    struct { type center_x; type center_y; type radius; };  \
    type v[3];                                  \
  } Circle##suffix;                             \
  Circle_IMPL(type, suffix)

#define Triangle_DEF(type, suffix)              \
  typedef union {                               \
    struct {                                    \
      type a_x; type a_y;                       \
      type b_x; type b_y;                       \
      type c_x; type c_y;                       \
    };                                          \
    type v[6];                                  \
  } Triangle##suffix;                           \
  Triangle_IMPL(type, suffix)

  
#else // < C11
// These definitions do not use unions
  
#define Vec2_DEF(type, suffix)                    \
  typedef struct {                                \
    type x;                                       \
    type y;                                       \
  } Vec2##suffix;                                 \
  Vec2_IMPL(type, suffix)

#define Vec3_DEF(type, suffix)                    \
  typedef struct {                                \
    type x;                                       \
    type y;                                       \
    type z;                                       \
  } Vec3##suffix;                                 \
  Vec3_IMPL(type, suffix)

#define Vec4_DEF(type, suffix)                    \
  typedef struct {                                \
    type a;                                       \
    type b;                                       \
    type c;                                       \
    type d;                                       \
  } Vec4##suffix;                                 \
  Vec4_IMPL(type, suffix)
  
#define Rect_DEF(type, suffix)                  \
  typedef struct {                              \
    type a_x; type a_y;                         \
    type b_x; type b_y;                         \
    type c_x; type c_y;                         \
    type d_x; type d_y;                         \
  } Rect##suffix;                               \
  Rect_IMPL(type, suffix)

#define Circle_DEF(type, suffix)                \
  typedef struct {                              \
    type c_x; type c_y;                         \
    type r;                                     \
  } Circle##suffix;                             \
  Circle_IMPL(type, suffix)

#define Triangle_DEF(type, suffix)              \
  typedef struct {                              \
    type a_x; type a_y;                         \
    type b_x; type b_y;                         \
    type c_x; type c_y;                         \
  } Triangle##suffix;                           \
  Triangle_IMPL(type, suffix)
  
#endif // __STDC_VERSION__ >= 201112L // C11

Vec2_DEF(int, i)
Vec2_DEF(unsigned int, u)
Vec2_DEF(long long int, l)
Vec2_DEF(unsigned long long int, ul)
Vec2_DEF(float, f)
Vec2_DEF(double, d)

Vec3_DEF(int, i)
Vec3_DEF(unsigned int, u)
Vec3_DEF(long long int, l)
Vec3_DEF(unsigned long long int, ul)
Vec3_DEF(float, f)
Vec3_DEF(double, d)

Vec4_DEF(int, i)
Vec4_DEF(unsigned int, u)
Vec4_DEF(long long int, l)
Vec4_DEF(unsigned long long int, ul)
Vec4_DEF(float, f)
Vec4_DEF(double, d)

Rect_DEF(int, i)
Rect_DEF(unsigned int, u)
Rect_DEF(long long int, l)
Rect_DEF(unsigned long long int, ul)
Rect_DEF(float, f)
Rect_DEF(double, d)

Circle_DEF(int, i)
Circle_DEF(unsigned int, u)
Circle_DEF(long long int, l)
Circle_DEF(unsigned long long int, ul)
Circle_DEF(float, f)
Circle_DEF(double, d)

Triangle_DEF(int, i)
Triangle_DEF(unsigned int, u)
Triangle_DEF(long long int, l)
Triangle_DEF(unsigned long long int, ul)
Triangle_DEF(float, f)
Triangle_DEF(double, d)

//
// Implementation
//
  
#ifdef MICRO_LA_IMPLEMENTATION

MICRO_LA_DEF Quaternion micro_la_quaternion_prod(Quaternion p, Quaternion q)
{
  return (Quaternion) {
    .a = p.a * q.a - p.b * q.b - p.c * q.c - p.d * q.d,
    .b = p.a * q.b + p.b * q.a + p.c * q.d - p.d * q.c,
    .c = p.a * q.c - p.b * q.d + p.c * q.a + p.d * q.b,
    .d = p.a * q.d + p.b * q.c - p.c * q.b + p.d * q.a,
  };
}

MICRO_LA_DEF Quaternion micro_la_quaternion_recip(Quaternion p)
{
  double q = p.a * p.a + p.b * p.b + p.c * p.c + p.d * p.d;        
  return (Quaternion) {
    .a = p.a / q,
    .b = -p.b / q,
    .c = -p.c / q,
    .d = -p.d / q
  };
}

// For now, only rotate over the z axis
//
// Very useful resource to understand quaternions:
// - https://eater.net/quaternions
// - https://www.cs.ucdavis.edu/~amenta/3dphoto/quaternion.pdf
MICRO_LA_DEF Quaternion
micro_la_quaternion_rotate(Quaternion vec_quaternion, double angle)
{
  Quaternion rot_quaternion =
    (Quaternion){ .a = cos(angle / 2), .b = 0, .c = 0, .d = sin(angle / 2)};
  Quaternion rot_quaternion_recip =
    micro_la_quaternion_recip(rot_quaternion);
  Quaternion first_prod =
    micro_la_quaternion_prod(rot_quaternion, vec_quaternion);
  return micro_la_quaternion_prod(first_prod, rot_quaternion_recip);
}

#endif // MICRO_LA_IMPLEMENTATION
 
#ifdef __cplusplus
}
#endif

#endif // MICRO_LA
