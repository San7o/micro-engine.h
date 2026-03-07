# ====================================================================
# Project:   raytracer
# Author:    Giovanni Santini
# Mail:      giovanni.santini@proton.me
# Github:    @San7o
# License:   MIT
# ====================================================================

cmake_minimum_required(VERSION 3.16)

project(micro-engine
  VERSION 1.0
  DESCRIPTION "A micro game engine"
  LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # no gnu extensions

find_program(CCACHE_PROGRAM ccache) # use ccache if available
if(CCACHE_PROGRAM)
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

if(MSVC)
  set(WARNING_FLAGS /W4 /permissive-)
  set(DEBUG_FLAGS /Zi /Od)
  set(RELEASE_FLAGS /O2)
  set(DEBUG_DEFINES _DEBUG)
elseif(MINGW)
  set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
  set(DEBUG_FLAGS -O0 -ggdb)
  set(RELEASE_FLAGS -O3)
  set(DEBUG_DEFINES DEBUG)
else()
  set(WARNING_FLAGS -Wall -Wextra -Werror -Wpedantic)
  set(DEBUG_FLAGS -O0 -ggdb -fsanitize=address)
  set(RELEASE_FLAGS -O3)
  set(DEBUG_DEFINES DEBUG)
  set(DEBUG_LINK_FLAGS -fsanitize=address)
endif()

#
# Options
#

option(MICRO_ENGINE_BUILD_TESTS    "Build tests"    ON)
option(MICRO_ENGINE_BUILD_EXAMPLES "Build examples" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release)" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(MICRO_ENGINE_COMPILE_OPTIONS
    ${WARNING_FLAGS}
    ${DEBUG_FLAGS}
    ${DEBUG_DEFINES}
    ${DEBUG_LINK_FLAGS})
else()
  set(MICRO_ENGINE_COMPILE_OPTIONS
    ${WARNING_FLAGS}
    ${RELEASE_FLAGS})
endif()

#
# Examples
#

set(MICRO_ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(MICRO_ENGINE_LINK_LIBS -lm -lX11 -lXrandr)

function(add_example NAME)
  message(STATUS "Added example ${NAME}")
  
  add_executable(${NAME}
    ${PROJECT_SOURCE_DIR}/examples/${PROJECT_NAME}-impl.c
    ${PROJECT_SOURCE_DIR}/examples/${NAME}.c)
  target_include_directories(${NAME} PRIVATE
    ${MICRO_ENGINE_INCLUDE_DIRS})
  target_link_libraries(${NAME} PRIVATE
    ${MICRO_ENGINE_LINK_LIBS})
  target_compile_options(${NAME} PRIVATE
    ${MICRO_ENGINE_COMPILE_OPTIONS})
endfunction()

if (MICRO_ENGINE_BUILD_EXAMPLES)
  add_example(demo)
endif()

#
# End of CMakeLists.txt
# ====================================================================
