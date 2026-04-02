///////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-la.h
// ==========
//
// Linear algebra types and functions as an header-only library with a
// C99 and C11 api.
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
//   - Rect
//   - Circle
//   - Triangle
//
// And the math functions for:
//
//   - floor
//   - sqrt
//   - sin / cos / tan / arcsin / arccos / arctan
//   - abs
//   - quaternions
//
//
// Usage
// -----
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
//  - TODO: matrices
//  - TODO: implement quaternion rotations for any axes
//  - TODO: add rotations to shapes Rect and Triangle
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

//
// Quaternions
// -----------
//
// Quaternions are used to represent rotations, generally written in
// the form:
//
//                   a + bi + cj + dk
//
// You can think of the unit vector (b, c, d) being the axis of
// rotation, with a being the rotation.
//
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

//
// PI
// --
//
// The number PI, that is the ratio of a circle's circumference to its
// diameter:
//
//                           pi = C / d
//
// The first recorded algorithm for rigorously calculating the value
// of PI was a geometrical approach using polygons, devised around 250
// BC by the Greek mathematician Archimedes, implementing the method
// of exhaustion. This polygonal algorithm dominated for over 1,000
// years, and as a result PI is sometimes referred to as Archimedes's
// constant. Archimedes computed upper and lower bounds of PI by
// drawing a regular hexagon inside and outside a circle, and
// successively doubling the number of sides until he reached a
// 96-sided regular polygon. By calculating the perimeters of these
// polygons, he proved that ⁠223/71 < PI < 22/7.
//
// The calculation of PI was revolutionized by the development of
// infinite series techniques in the 16th and 17th centuries. In the
// 1660s, Isaac Newton and Leibniz discovered calculus, which led to
// the development of many infinite series for approximating PI.
//
// One way to approximate PI is by using the taylor series for
// calculating arctan(1) which is equal to PI / 4. Other series were
// discovered that approac PI faster than arctan(x).
#ifndef PI
  #define PI 3.14159265358979323846f
#endif

//
// Euler's number
// --------------
//
// It can be defined in many ways, for example as the sum of the
// infinite series:
//
//                     \sum_{n=0}^{\inf} 1 / n!
//
// Also e = exp(1), where exp is the natural exponential function, the
// unique function that equals its own derivative and satisfies the
// equation exp(0) = 1.
//
// Example: let's take the function 2^t. We know its derivative is
// defined as the limit of dt->0 of:
//
//                      (2^(t + dt) - 2^t) / dt
//
// We use the properties of the exponential 2^(t + dt) = 2^t * 2 ^ dt:
//
//                     (2^t * 2 ^ dt - 2^t) / dt
//
// We factor out 2^t:
//
//                    (2^t * ((2 ^ dt - 1) / dt))
//
// The ((2 ^ dt - 1) / dt) actually converges to an real number, being
// 0.6931... This number is different for each base. Then, what would
// the base have to be so that this constant is equal to 1? Meaning,
// what is the base such that its derivative is itself? That is
// exactly the euler number E. The natural logarithm is defined as the
// inverse of the exponential with base E.
//
// Euler's number is famously used in euler's formula:
//
//                 e ^ {i * x} = cos(x) + i * sin(x)
//
#ifndef EULER
  #define EULER 2.7182818284590452353f
#endif

static inline float micro_floorf(float x)
{
  if (x >= 0.0f)
    return (float)((int)x);
  
  int i = (int)x;
  float f = (float)i;
  // If x is negative and not already a whole integer, 
  // the cast truncated "up" toward zero, so we subtract 1.
  if (f > x)
    return f - 1.0f;
  return f;
}

static inline float micro_sqrtf(float x)
{
    if (x <= 0) return 0;

    // Initial guess (the closer the better, but x/2 works)
    float guess = x;
    
    // 5 iterations is usually enough for float precision
    for (int i = 0; i < 5; i++) {
        guess = 0.5f * (guess + x / guess);
    }

    return guess;
}
  
//
// Taylor series
// -------------
//
// We can approximate a non-polinomial function with a polinomial
// function near a certain point using the Taylor series, this is much
// easier to compute and work with. The series is:
//
//           \sum_{n=0}^{\inf} f^{(n)}(a)/n! * (x-a)^n
//
// Member N of the polinomial essentially matches derivative of degree
// N of the non-polinomial function. A Taylor series centered at 0
// is known as Maclaurin series.
static inline float micro_sin(float x)
{
  // Fast range reduction for large x, to map x to [-PI, PI]
  x = x - (2.0f * PI) * micro_floorf((x + PI) / (2.0f * PI));

  // Taylor Series (7 terms for high precision)
  float x2 = x * x;
  float term = x;
  float sum = x;
    
  // We calculate terms iteratively to avoid large factorial divisions
  term *= -x2 / (2 * 3); sum += term; // x^3
  term *= -x2 / (4 * 5); sum += term; // x^5
  term *= -x2 / (6 * 7); sum += term; // x^7
  term *= -x2 / (8 * 9); sum += term; // x^9
    
  return sum;
}

static inline float micro_cos(float x)
{
  return micro_sin(x + 1.57079632f); // x + PI/2
}

static inline float micro_tan(float x)
{
  return micro_sin(x) / micro_cos(x);
}

static inline float micro_arctan(float x)
{
  // 1. Handle range reduction to |x| <= 1
  if (x > 1.0f)  return (PI / 2.0f) - micro_arctan(1.0f / x);
  if (x < -1.0f) return (-PI / 2.0f) - micro_arctan(1.0f / x);

  // 2. Taylor series: x - x^3/3 + x^5/5 - x^7/7 + x^9/9
  float x2 = x * x;
  float x_pow = x;
  float sum = x;

  x_pow *= -x2; sum += x_pow / 3.0f;
  x_pow *= -x2; sum += x_pow / 5.0f;
  x_pow *= -x2; sum += x_pow / 7.0f;
  x_pow *= -x2; sum += x_pow / 9.0f;

  return sum;
}

static inline float micro_arccos(float x)
{
  // Clamp input to valid range [-1, 1] to avoid NaN
  if (x <= -1.0f) return PI;
  if (x >= 1.0f)  return 0.0f;

  // Use the arctan identity for better stability
  float val = micro_sqrtf(1.0f - x * x) / (1.0f + x);
  return 2.0f * micro_arctan(val);
}

static inline float micro_arcsin(float x)
{
  // Clamp to valid domain [-1, 1]
  if (x >= 1.0f) return PI / 2.0f;
  if (x <= -1.0f) return -PI / 2.0f;

  // Use the arctan identity: arcsin(x) = arctan(x / sqrt(1-x^2))
  float denom = micro_sqrtf(1.0f - x * x);
  return micro_arctan(x / denom);
}

#ifndef micro_abs
#define micro_abs(x) ((x) > 0) ? (x) : -(x)
#endif
  
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Vec2##suffix##_dot(Vec2##suffix a, Vec2##suffix b)        \
  {                                                         \
    return a.x * b.x + a.y * b.y;                           \
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Vec2##suffix##_cross(Vec2##suffix a, Vec2##suffix b)      \
  {                                                         \
    return (a.x * b.y) - (a.y * b.x);                       \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec2##suffix##_add(Vec2##suffix *a, Vec2##suffix b)       \
  {                                                         \
    a->x += b.x;                                            \
    a->y += b.y;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec2##suffix##_sub(Vec2##suffix *a, Vec2##suffix b)       \
  {                                                         \
    a->x -= b.x;                                            \
    a->y -= b.y;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF bool                                         \
  Vec2##suffix##_eq(Vec2##suffix a, Vec2##suffix b)         \
  {                                                         \
    return (a.x == b.x && a.y == b.y);                      \
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Vec3##suffix##_dot(Vec3##suffix a, Vec3##suffix b)        \
  {                                                         \
    return a.x * b.x + a.y * b.y + a.z * b.z;               \
  }                                                         \
                                                            \
  MICRO_LA_DEF Vec3##suffix                                 \
  Vec3##suffix##_cross(Vec3##suffix a, Vec3##suffix b)      \
  {                                                         \
    Vec3##suffix result;                                    \
    result.x = (a.y * b.z) - (a.z * b.y);                   \
    result.y = (a.z * b.x) - (a.x * b.z);                   \
    result.z = (a.x * b.y) - (a.y * b.x);                   \
    return result;                                          \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec3##suffix##_add(Vec3##suffix *a, Vec3##suffix b)       \
  {                                                         \
    a->x += b.x;                                            \
    a->y += b.y;                                            \
    a->z += b.z;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec3##suffix##_sub(Vec3##suffix *a, Vec3##suffix b)       \
  {                                                         \
    a->x -= b.x;                                            \
    a->y -= b.y;                                            \
    a->z -= b.z;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF bool                                         \
  Vec3##suffix##_eq(Vec3##suffix a, Vec3##suffix b)         \
  {                                                         \
    return (a.x == b.x && a.y == b.y && a.z == b.z);        \
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Vec4##suffix##_dot(Vec4##suffix a, Vec4##suffix b)        \
  {                                                         \
    return a.a * b.a + a.b * b.b + a.c * b.c + a.d * b.d;   \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec4##suffix##_add(Vec4##suffix *a, Vec4##suffix b)       \
  {                                                         \
    a->a += b.a;                                            \
    a->b += b.b;                                            \
    a->c += b.c;                                            \
    a->d += b.d;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF void                                         \
  Vec4##suffix##_sub(Vec4##suffix *a, Vec4##suffix b)       \
  {                                                         \
    a->a -= b.a;                                            \
    a->b -= b.b;                                            \
    a->c -= b.c;                                            \
    a->d -= b.d;                                            \
  }                                                         \
                                                            \
  MICRO_LA_DEF bool                                         \
  Vec4##suffix##_eq(Vec4##suffix a, Vec4##suffix b)         \
  {                                                         \
    return (a.a == b.a && a.b == b.b && a.c == b.c && a.d == b.d);  \
  }

//
// C +-----+ B
//   |     |
//   |     |
//   |     |
// D +-----+ A
//
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Rect##suffix##_area(Rect##suffix rect)                    \
  {                                                         \
    float delta_base_x   = rect.a_x - rect.b_x;             \
    float delta_base_y   = rect.a_y - rect.b_y;             \
    float delta_height_x = rect.b_x - rect.c_x;             \
    float delta_height_y = rect.b_y - rect.c_y;             \
    float base = micro_sqrtf(delta_base_x * delta_base_x    \
                             + delta_base_y * delta_base_y);     \
    float height = micro_sqrtf(delta_height_x * delta_height_x   \
                               + delta_height_y * delta_height_y);  \
    return base * height;                                           \
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Circle##suffix##_circumference(Circle##suffix circle)     \
  {                                                         \
    return 2 * PI * circle.r;                               \
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Circle##suffix##_area(Circle##suffix circle)              \
  {                                                         \
    return PI * circle.r * circle.r;                        \
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
  }                                                         \
                                                            \
  MICRO_LA_DEF float                                        \
  Triangle##suffix##_area(Triangle##suffix triangle)        \
  {                                                         \
    return (triangle.b_x - triangle.a_x)                    \
      * (triangle.c_y - triangle.a_y)                       \
      - (triangle.c_x - triangle.a_x)                       \
      * (triangle.b_x - triangle.a_y);                      \
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
Vec2_DEF(float, f)
// Vec2_DEF(unsigned int, u)
// Vec2_DEF(long long int, l)
// Vec2_DEF(unsigned long long int, ul)
// Vec2_DEF(double, d)

Vec3_DEF(int, i)
Vec3_DEF(float, f)
// Vec3_DEF(unsigned int, u)
// Vec3_DEF(long long int, l)
// Vec3_DEF(unsigned long long int, ul)
// Vec3_DEF(double, d)

Vec4_DEF(int, i)
Vec4_DEF(float, f)
// Vec4_DEF(unsigned int, u)
// Vec4_DEF(long long int, l)
// Vec4_DEF(unsigned long long int, ul)
// Vec4_DEF(double, d)

Rect_DEF(int, i)
Rect_DEF(float, f)
// Rect_DEF(unsigned int, u)
// Rect_DEF(long long int, l)
// Rect_DEF(unsigned long long int, ul)
// Rect_DEF(double, d)

Circle_DEF(int, i)
Circle_DEF(float, f)
// Circle_DEF(unsigned int, u)
// Circle_DEF(long long int, l)
// Circle_DEF(unsigned long long int, ul)
// Circle_DEF(double, d)

Triangle_DEF(int, i)
Triangle_DEF(float, f)
// Triangle_DEF(unsigned int, u)
// Triangle_DEF(long long int, l)
// Triangle_DEF(unsigned long long int, ul)
// Triangle_DEF(double, d)

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
    (Quaternion){ .a = micro_cos(angle / 2), .b = 0, .c = 0, .d = micro_sin(angle / 2)};
  Quaternion rot_quaternion_recip =
    micro_la_quaternion_recip(rot_quaternion);
  Quaternion first_prod =
    micro_la_quaternion_prod(rot_quaternion, vec_quaternion);
  return micro_la_quaternion_prod(first_prod, rot_quaternion_recip);
}

#ifdef __cplusplus
}
#endif

#endif // MICRO_LA
