// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef MICRO_TYPES_H
#define MICRO_TYPES_H

#ifndef size_t
  typedef long unsigned int  size_t;
#endif

#ifndef true
  #define true  1
#endif
#ifndef false
  #define false 0
#endif

#ifndef bool
  typedef _Bool bool;
#endif

#ifndef NULL
  #define NULL ((void*) 0)
#endif

#ifndef EXIT_SUCCESS
  #define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
  #define EXIT_FAILURE 1
#endif

#endif // MICRO_TYPES_H
