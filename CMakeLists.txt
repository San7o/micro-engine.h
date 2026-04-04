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

if (EMSCRIPTEN)
  set(CMAKE_C_STANDARD 23)
  set(CMAKE_C_EXTENSIONS ON)
else()
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_C_EXTENSIONS OFF)  # no gnu extensions
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)

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
  if (EMSCRIPTEN)
    set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
  else()
    set(WARNING_FLAGS -Wall -Werror -Wextra -Wpedantic)
  endif()
  set(DEBUG_FLAGS -O0 -ggdb)
  set(RELEASE_FLAGS -O3)
  set(DEBUG_DEFINES _DEBUG)
else()
  if (EMSCRIPTEN)
    set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
  else()
    set(WARNING_FLAGS -Wall -Wextra -Werror -Wpedantic)
  endif()
  set(DEBUG_FLAGS -O0 -ggdb -fsanitize=address)
  set(RELEASE_FLAGS -O3)
  set(DEBUG_DEFINES _DEBUG)
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
  message(STATUS "Configuring Debug build options...")

  set(MICRO_ENGINE_COMPILE_DEFINES ${DEBUG_DEFINES})
  set(MICRO_ENGINE_COMPILE_OPTIONS ${DEBUG_FLAGS} ${WARNING_FLAGS})
  if (NOT MSVC)
    set(MICRO_ENGINE_LINK_OPTIONS ${DEBUG_LINK_FLAGS})
  endif()
  
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Configuring Release build options...")
  set(MICRO_ENGINE_COMPILE_OPTIONS
    ${RELEASE_FLAGS}
    ${WARNING_FLAGS})
endif()

#
# Examples
#

set(MICRO_ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(MICRO_ENGINE_LINK_LIBS -lm -lX11)

add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCE_DIR}/examples/${PROJECT_NAME}-impl.c)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${MICRO_ENGINE_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE
  ${MICRO_ENGINE_DEBUG_DEFINES})
target_compile_options(${PROJECT_NAME} PRIVATE
  ${MICRO_ENGINE_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PRIVATE
  ${MICRO_ENGINE_LINK_OPTIONS})

add_library(rgfw_platform STATIC
  ${PROJECT_SOURCE_DIR}/examples/rgfw_platform-impl.c)
target_include_directories(rgfw_platform PRIVATE
  ${MICRO_ENGINE_INCLUDE_DIRS})
target_link_libraries(rgfw_platform Xrandr X11)

function(add_example NAME ADDITIONAL_LINK_LIBS)
  message(STATUS "Added example ${NAME}")
  
  add_executable(${NAME}
    ${PROJECT_SOURCE_DIR}/examples/${NAME}.c)

  target_include_directories(${NAME} PRIVATE
    ${MICRO_ENGINE_INCLUDE_DIRS})
  target_compile_definitions(${NAME} PRIVATE
    ${MICRO_ENGINE_DEBUG_DEFINES})
  target_link_libraries(${NAME} PRIVATE
    micro-engine
    ${MICRO_ENGINE_LINK_LIBS} ${ADDITIONAL_LINK_LIBS})
  target_compile_options(${NAME} PRIVATE
    ${MICRO_ENGINE_COMPILE_OPTIONS})
  target_link_options(${NAME} PRIVATE
    ${MICRO_ENGINE_LINK_OPTIONS})

  if(EMSCRIPTEN)
    set_target_properties(${NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${NAME} PRIVATE
      #"-sUSE_SDL=3"
      "SHELL:--shell-file \"${PROJECT_SOURCE_DIR}/utils/wasm_template.html\""
      "-sEXPORTED_RUNTIME_METHODS=[\"HEAPU8\",\"stringToNewUTF8\"]"
      #"-sEXPORT_ALL=1"
      "-sASYNCIFY"
      "-sASSERTIONS"
    )
  endif()
  
endfunction()

if (MICRO_ENGINE_BUILD_EXAMPLES)
  if (EMSCRIPTEN)
    add_example(app_example "")
    add_example(raycaster "")
    add_example(mandelbrot "")
    add_example(bezier_curve "")
  else()
    add_example(sdl3_demo SDL3)
    add_example(fft_visualizer SDL3)
    add_example(rgfw_demo rgfw_platform)
    add_example(load_image Xrandr)
    add_example(app_example rgfw_platform)
    add_example(raycaster rgfw_platform)
    add_example(mandelbrot rgfw_platform)
    add_example(cosu rgfw_platform)
    add_example(bezier_curve rgfw_platform)
  endif()
endif()

#
# End of CMakeLists.txt
# ====================================================================
