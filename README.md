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

## Motivation

As Brenta grew in complexity, it naturally became more cumbersome to
quickly design and implement new features and applications. This is a
fundamental problem of all software, not a fault of my engine. Hence I
wanted to experiment with a simpler approach. I mean "simple" in its
most general form: in the abstraction, workflow and implementation.

![](./docs/micro-website/micro-engine-marketing.jpg)
