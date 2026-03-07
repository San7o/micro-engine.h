///////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-conf.h
// ============
//
// Header-only library to parse config files, in C99.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o
//
//
// Example
// -------
//
//    // Example of a struct holding some values
//    typedef struct {
//      int x;
//      int y;
//    } Vec2;
//
//    int main(void)
//    {
//      Vec2 vec = (Vec2) {
//        .x = 1,
//        .y = 1,
//      };
//
//      // Define which variables should be parsed in the config file
//      // and their symbols
//      MicroConf config[] =
//        {
//        // type             variable      symbol
//          {MICRO_CONF_INT,   &vec.x,     "vec.x"},
//          {MICRO_CONF_INT,   &vec.y,     "vec.y"},
//        };
//      size_t num_conf = sizeof(config) / sizeof(config[0]);
//
//      // Parse the file
//      int err = micro_conf_parse(config, num_conf, "micro.conf");
//      if (err < 0) return -err;
//    
//      return 0;
//    }
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_CONF_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_CONF_IMPLEMENTATION
//   #include "micro-conf.h"
//
// This library lets you define which variable to parse from a config
// file. You do so by creating an array of MicroConf, for example:
//
//    MicroConf config[] =
//      {
//        {MICRO_CONF_INT, &x, "x"},
//      };
//
// Here, &x is a pointer to a variable that will be set according to
// the value present in the configuration file, if present, and "x" is
// the key of this value.
//
// The configuration file is a list of key-value pairs separated by
// either a `=` or `:`
//
//    x:  10
//    x = 10
//
// The number of spaces is irrelevant. You can parse a config file
// with `micro_conf_parse` by specifying the configs, the number of
// entries, and the filename to parse. For example:
//
//    micro_conf_parse(config, num_conf, "micro.conf");
//    if (err != MICRO_CONF_OK) return -err;
//
//
// Code
// ----
//
// The official git repository of micro-conf.h is hosted at:
//
//     https://github.com/San7o/micro-conf.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//

#ifndef MICRO_CONF
#define MICRO_CONF

#define MICRO_CONF_MAJOR 0
#define MICRO_CONF_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Confuration
//

// Conf: Prefix for all functions
// For function inlining, set this to `static inline` and then define
// the implementation in all the files
#ifndef MICRO_CONF_DEF
  #define MICRO_CONF_DEF extern
#endif
  
//
// Macros
//

//
// Errors
//

#define MICRO_CONF_OK                     0
#define MICRO_CONF_ERROR_CONF_NULL       -1
#define MICRO_CONF_ERROR_OPENING_FILE    -2
#define MICRO_CONF_ERROR_CLOSING_FILE    -3
#define MICRO_CONF_ERROR_UNKNOWN_TYPE    -4
#define MICRO_CONF_ERROR_INVALID_BOOL    -5
#define MICRO_CONF_ERROR_INVALID_INT     -6
#define MICRO_CONF_ERROR_INVALID_DOUBLE  -7
#define MICRO_CONF_ERROR_INVALID_FLOAT   -8
#define MICRO_CONF_ERROR_INVALID_CHAR    -9
#define _MICRO_CONF_ERROR_MAX            -10

//
// Types
//

typedef enum {
  MICRO_CONF_BOOL,
  MICRO_CONF_INT,
  MICRO_CONF_FLOAT,
  MICRO_CONF_DOUBLE,
  MICRO_CONF_CHAR,
  MICRO_CONF_STR,
} MicroConfType;
  
typedef struct {
  MicroConfType type;
  void* value;
  char* name;
} MicroConf;

//
// Function declarations
//

#include <stddef.h>
#include <stdbool.h>

// Parse [pathname] with a specified [conf] of [num_conf] values
// Returns MICRO_CONF_OK on success, or a MICRO_CONF_ERROR_ otherwise
//
// Note: MICRO_CONF_STR allocates memory for the parsed string, you
// should free it
MICRO_CONF_DEF int
micro_conf_parse(MicroConf *conf, size_t num_conf, const char *pathname);
  
//
// Implementation
//
  
#ifdef MICRO_CONF_IMPLEMENTATION

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get the number of separator characters from the left of [input]
// until the first non-separator or [input_size]. Separator characters
// are specs, new lines and tabs. Updates [line] and/or [column] if
// non null.
int left_space(const char* input, int input_size,
               unsigned int* line, unsigned int* column)
{
  if (!input || input_size == 0) return 0;

  int pos = 0;
  while(pos < input_size &&
        (input[pos] == ' ' ||
         input[pos] == '\n' ||
         input[pos] == '\t'))
  {
    if (input[pos] == '\n') {
      if (line) (*line)++;
      if (column) *column = 0;
    } else {
      if (column) (*column)++;
    }
    pos++;
  }

  return pos;
}
  
MICRO_CONF_DEF int
micro_conf_parse(MicroConf *conf, size_t num_conf, const char *pathname)
{
  if (!conf) return MICRO_CONF_ERROR_CONF_NULL;

  FILE *file = fopen(pathname, "r");
  if (!file) return MICRO_CONF_ERROR_OPENING_FILE;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, file)) >= 0)
  {
    char *comment = strchr(line, '#');
    if (comment) *comment = '\0';

    int space = left_space(line, (int)read, NULL, NULL);
    char *trimmed = line + space;

    if (*trimmed == '\0' || *trimmed == '\n') continue;

    for (size_t i = 0; i < num_conf; ++i)
    {
      size_t name_len = strlen(conf[i].name);
      if (strncmp(trimmed, conf[i].name, name_len) != 0)
        continue;

      char *after_name = trimmed + name_len;
      space = left_space(after_name, (int)(read - space - name_len), NULL, NULL);
      after_name += space;

      if (*after_name == '=' || *after_name == ':') after_name++;
      space = left_space(after_name, (int)(read - (after_name - line)), NULL, NULL);
      after_name += space;

      char *value_str = after_name;
      size_t vlen = strlen(value_str);
      while (vlen > 0 && (value_str[vlen-1] == '\n' || value_str[vlen-1] == ' '))
        value_str[--vlen] = '\0';

      switch (conf[i].type)
      {
      case MICRO_CONF_BOOL:
      {
        if (strcmp(value_str, "true") == 0 || strcmp(value_str, "1") == 0)
        {
          *((bool*)conf[i].value) = true;
        }
        else if (strcmp(value_str, "false") == 0 || strcmp(value_str, "0") == 0)
        {
          *((bool*)conf[i].value) = false;
        }
        else
        {
          free(line);
          fclose(file);
          return MICRO_CONF_ERROR_INVALID_BOOL;
        }
        break;
      }
      case MICRO_CONF_CHAR:
      {
        if (strlen(value_str) == 1)
        {
          *((char*)conf[i].value) = value_str[0];
        }
        else
        {
          free(line);
          fclose(file);
          return MICRO_CONF_ERROR_INVALID_CHAR;
        }
        break;
      }
      case MICRO_CONF_STR:
      {
        *((char**)conf[i].value) = strdup(value_str);
        break;
      }
      case MICRO_CONF_INT:
      {
        char *endptr;
        long val = strtol(value_str, &endptr, 10);
        if (*endptr != '\0')
        {
          free(line);
          fclose(file);
          return MICRO_CONF_ERROR_INVALID_INT;
        }
        *((int*)conf[i].value) = (int)val;
        break;
      }
      case MICRO_CONF_DOUBLE:
      {
        char *endptr;
        double val = strtod(value_str, &endptr);
        if (*endptr != '\0')
        {
          free(line);
          fclose(file);
          return MICRO_CONF_ERROR_INVALID_DOUBLE;
        }
        *((double*)conf[i].value) = val;
        break;
      }
      case MICRO_CONF_FLOAT:
      {
        char *endptr;
        double val = strtof(value_str, &endptr);
        if (*endptr != '\0')
        {
          free(line);
          fclose(file);
          return MICRO_CONF_ERROR_INVALID_FLOAT;
        }
        *((float*)conf[i].value) = val;
        break;
      }
      default:
        free(line);
        fclose(file);
        return MICRO_CONF_ERROR_UNKNOWN_TYPE;
      }

      break;
    }
  }

  free(line);
  if (fclose(file) != 0) return MICRO_CONF_ERROR_CLOSING_FILE;
  return MICRO_CONF_OK;
}
  
#endif // MICRO_CONF_IMPLEMENTATION

//
// Examples
//
  
#if 0

#define MICRO_CONF_IMPLEMENTATION
#include "micro-conf.h"

#include <assert.h>

typedef struct {
  int x;
  int y;
} Vec2;
  
typedef struct {
  int an_integer;
  float a_float;
  double a_double;
  bool a_bool;
  char a_char;
  char *a_str;
  Vec2 vec;
} MyConf;

int main(void)
{
  MyConf conf;
  // Defaults
  conf.an_integer = 10;
  conf.a_float = 11.0f;
  conf.a_double = 123.123;
  conf.a_bool = true;
  conf.a_char = 'F';
  conf.a_str = "test";
  conf.vec = (Vec2) {
    .x = 1,
    .y = 1,
  };
  
  MicroConf config[] =
    {
      {MICRO_CONF_INT, &conf.an_integer, "an_integer"},
      {MICRO_CONF_FLOAT, &conf.a_float, "a_float"},
      {MICRO_CONF_DOUBLE, &conf.a_double, "a_double"},
      {MICRO_CONF_BOOL, &conf.a_bool, "a_bool"},
      {MICRO_CONF_CHAR, &conf.a_char, "a_char"},
      {MICRO_CONF_STR, &conf.a_str, "a_str"},
      {MICRO_CONF_INT, &conf.vec.x, "vec.x"},
      {MICRO_CONF_INT, &conf.vec.y, "vec.y"},
    };
  size_t num_conf = sizeof(config) / sizeof(config[0]);
  
  int err = micro_conf_parse(config, num_conf, "micro.conf");
  if (err != MICRO_CONF_OK) return -err;

  // ...
  
  return 0;
}
  
#endif // 0
  
#ifdef __cplusplus
}
#endif

#endif // MICRO_CONF
