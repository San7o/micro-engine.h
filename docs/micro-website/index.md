---
title: micro-engine
description: A micro game engine
---

# micro-engine.h

micro-engine is an header-only game engine. It is composed of several
independent headers from the beautiful
[microheaders](https://san7o.github.io/micro-headers/) collection.

Unlike my other game engine,
[Brenta-Engine](https://san7o.github.io/Brenta-Engine/), micro-engine
favors absolute simplicity over powerful but complex features. It is
written in C99, graphics are software rendered with a focus on 2D, and
it has no dependencies.

## Features

- high portability
  - plug your custom memory allocator (`micro-arena.h` is used by
    default)
  - pltform is abstracted in `micro-platform.h`, you need to implement
    just a few functions to port the engine. See
    [platforms](./include/micro-engine/platforms)
  - no dependencies
  - works everywhere C can compile to
- modularity: everything can be used as a stand-alone header-only
    library, meaning that you can use any part of the engine in your
    projects
- simplicity: you have a raw buffer where you can draw pixel to, the
  rest is in your control
- [microheaders](https://san7o.github.io/micro-headers/) included:
  - [micro-draw.h](https://github.com/San7o/micro-draw.h)
  - [micro-log.h](https://github.com/San7o/micro-log.h)
  - [micro-la.h](https://github.com/San7o/micro-la.h)
  - [micro-arena.h](https://github.com/San7o/micro-arena.h)
  - [micro-tests.h](https://github.com/San7o/micro-tests.h)
  - [micro-serde.h](https://github.com/San7o/micro-serde.h)
  - [micro-flag.h](https://github.com/San7o/micro-flag.h)
  - [micro-module.h](https://github.com/San7o/micro-module.h)
  - [micro-conf.h](https://github.com/San7o/micro-conf.h)

