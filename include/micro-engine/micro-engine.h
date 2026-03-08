// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

//
// Micro engine
// ------------
//
// Import everything
//
// Define `MICRO_ENGINE_IMPLEMENTATION` for including the
// implementaion.
//

#ifndef MICRO_ENGINE
#define MICRO_ENGINE

#ifdef MICRO_ENGINE_IMPLEMENTATION
  #define MICRO_LOG_IMPLEMENTATION
  #define MICRO_DRAW_IMPLEMENTATION
  #define MICRO_FLAG_IMPLEMENTATION
  #define MICRO_CONF_IMPLEMENTATION
  #define MICRO_SERDE_IMPLEMENTATION

  // External
  #define RGFW_IMPLEMENTATION
  #define STB_IMAGE_IMPLEMENTATION
  #define STB_TRUETYPE_IMPLEMENTATION

#endif

#define RGFWDEF
#define MICRO_LA_IMPLEMENTATION

#include "micro-log.h"
#include "micro-la.h"
#include "micro-draw.h"
#include "micro-flag.h"
#include "micro-conf.h"
#include "micro-serde.h"

// External
#include "external/RGFW.h"
#include "external/stb_image.h"
#include "external/stb_truetype.h"

#endif // MICRO_ENGINE
