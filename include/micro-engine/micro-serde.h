//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-serde.h
// =============
//
// Header-only serialization library for C99, in ~150 lines of code.
//
// Features:
//   - Header-only: just include and use.
//   - Schema-based: one definition reused across all struct instances.
//   - Serialized data stored in network byte order, works on LE an BE
//   - Incredibly easy to use
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Documentation
// -------------
//
// micro-serde.h provides a simple way to serialize and deserialize
// plain C structs without requiring reflection, runtime type
// metadata, or generated code.
//
// The library defines a schema ("type definition") once per struct,
// describing each field’s type and offset. This schema can then be
// applied to any instance of that struct at runtime. Serialization
// and deserialization functions use these definitions to walk through
// the struct fields and convert them to or from a binary or textual
// representation.
//
// For example, you would define a schema for the struct Vec2 in the
// following way:
//
//    typedef struct {
//      int x;
//      unsigned int y;
//    } Vec2;
//
//    MicroSerdeDef vec2_def[] = {         // Schema definition
//      MICRO_SERDE_FIELD_INT(Vec2, x),    // Int value
//      MICRO_SERDE_FIELD_UINT(Vec2, y),   // Unsigned int value
//    };
//
// You can use this definition inside other definitions, like so:
//
//    typedef struct {
//      Vec2 vec;
//      char name[10];
//    } MyType;
//
//    MicroSerdeDef my_type_def[] = {
//      MICRO_SERDE_FIELD_CUSTOM(MyType, vec, vec2_def),
//      MICRO_SERDE_FIELD_STR(MyType, name, 10),
//    };
//
// Once you have created your serialization definitions, you can
// finally serialize and deserialize with the functions
// `micro_serde_serialize` and `micro_serde_deserialize`
//
//    int micro_serde_serialize(MicroSerdeDef *def,
//                              unsigned int field_count,
//                              const void* instance,
//                              char *out, unsigned int out_size);
//
//    int micro_serde_deserialize(MicroSerdeDef *def,
//                                unsigned int field_count,
//                                void* instance,
//                                const char *input,
//                                unsigned int input_size);
//
//        Args:
//        - def: the definition of the type
//        - field_count: the number of fields in the definition.
//          Can be calculated with the macro:
//
//              MICRO_SERDE_DEF_LEN(my_type_def)
//
//        - instance: a pointer to the variable that will be
//          serialized / deserialized
//        - input / output: an user-allocated buffer to store / read
//          the serialized data
//        - input_size: size of [input]
//
//        The functions will return a non-negative integer representing
//        the number of bytes serialized, or a negative MICRO_SERDE_ error
//
// For example, using the definitions in the above example:
//
//    MyType my_type_instance = (MyType) {  // Initialize some values
//      .vec = (Vec2) {
//        .x = 69,
//        .y = 420,
//      },
//    };
//    strcpy(my_type_instance.name, "Test");
//
//    // Create a buffer to store the serialized struct
//    unsigned int my_type_serialized_len = 100;
//    char* my_type_serialized = malloc(my_type_serialized_len);
//
//    // Serialze
//    micro_serde_serialize(my_type_def,
//                          MICRO_SERDE_DEF_LEN(my_type_def),
//                          &my_type_instance,
//                          my_type_serialized,
//                          my_type_serialized_len);
//
//    MyType my_type_instance2 = {0};
//    // Deserialize
//    micro_serde_deserialize(my_type_def,
//                            MICRO_SERDE_DEF_LEN(my_type_def),
//                            &my_type_instance2,
//                            my_type_serialized,
//                            my_type_serialized_len);
//
//    free(my_type_serialized);
//
// For a more sophisticated serialization library that works across
// different languages, see https://github.com/protobuf-c/protobuf-c
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_SERDE_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_SERDE_IMPLEMENTATION
//   #include "micro-serde.h"
//
//
// Code
// ----
//
// The official git repository of micro-serde.h is hosted at:
//
//     https://github.com/San7o/micro-serde.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//

#ifndef MICRO_SERDE
#define MICRO_SERDE

#define MICRO_SERDE_MAJOR 0
#define MICRO_SERDE_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

// Config: Prefix for all functions
// For function inlining, set this to `static inline` and then define
// the implementation in all the files
#ifndef MICRO_SERDE_DEF
  #define MICRO_SERDE_DEF extern
#endif

  
//
// Macros
//

#define MICRO_SERDE_OFFSET_OF(type, field) ((size_t)&(((type *)0)->field))

#define MICRO_SERDE_DEF_LEN(micro_serde_def)              \
  (sizeof(micro_serde_def) / sizeof(micro_serde_def[0]))

#define MICRO_SERDE_FIELD(struct_type, field_name, field_type_enum,     \
                          field_size, custom_def, custom_def_size )     \
  { field_type_enum, MICRO_SERDE_OFFSET_OF(struct_type, field_name),    \
    field_size, custom_def }

#define MICRO_SERDE_FIELD_INT(struct_type, field_name)                  \
  { MICRO_SERDE_INT, MICRO_SERDE_OFFSET_OF(struct_type, field_name),    \
    sizeof(int), 0 }

#define MICRO_SERDE_FIELD_UINT(struct_type, field_name)               \
  { MICRO_SERDE_UINT, MICRO_SERDE_OFFSET_OF(struct_type, field_name), \
      sizeof(unsigned int), 0 }

#define MICRO_SERDE_FIELD_STR(struct_type, field_name, max_len)         \
  { MICRO_SERDE_STR, MICRO_SERDE_OFFSET_OF(struct_type, field_name), max_len, 0 }

#define MICRO_SERDE_FIELD_CUSTOM(custom_type, field_name, custom_def)   \
  { MICRO_SERDE_CUSTOM, MICRO_SERDE_OFFSET_OF(custom_type, field_name),    \
    MICRO_SERDE_DEF_LEN(custom_def), custom_def }

// Errors

#define MICRO_SERDE_OK                        0
#define MICRO_SERDE_ERROR_DEF_NULL           -1
#define MICRO_SERDE_ERROR_ARG_NULL           -2
#define MICRO_SERDE_ERROR_UNKNOWN_TYPE       -3
#define MICRO_SERDE_ERROR_OUT_OF_SPACE       -4
#define MICRO_SERDE_ERROR_MISSING_CUSTOM_DEF -5

//
// Types
//

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef enum {
  MICRO_SERDE_INT = 0,
  MICRO_SERDE_UINT,
  MICRO_SERDE_STR,
  MICRO_SERDE_CUSTOM,
  _MICRO_SERDE_MAX,
} MicroSerdeFieldType;

struct MicroSerdeDef;
typedef struct MicroSerdeDef MicroSerdeDef;

struct MicroSerdeDef {
  MicroSerdeFieldType type;
  unsigned int offset;
  unsigned int size;
  MicroSerdeDef *custom_def;
};

//
// Function definitions
//

MICRO_SERDE_DEF int
micro_serde_serialize(MicroSerdeDef *def,
                      unsigned int field_count,
                      const void* instance,
                      char *out, unsigned int out_size);

MICRO_SERDE_DEF int
micro_serde_deserialize(MicroSerdeDef *def,
                        unsigned int field_count,
                        void* instance,
                        const char *input,
                        unsigned int input_size);

//
// Implementation
//

#ifdef MICRO_SERDE_IMPLEMENTATION

MICRO_SERDE_DEF int
micro_serde_serialize(MicroSerdeDef *def,
                      unsigned int field_count,
                      const void* instance,
                      char *out, unsigned int out_size)
{
  if (!def) return MICRO_SERDE_ERROR_DEF_NULL;
  if (!instance || !out) return MICRO_SERDE_ERROR_ARG_NULL;

  unsigned int offset = 0;
  for (unsigned int i = 0; i < field_count; ++i)
  {
    switch(def[i].type)
    {
    case MICRO_SERDE_INT: // fall
    case MICRO_SERDE_UINT:
      if (offset + def[i].size >= out_size)
        return MICRO_SERDE_ERROR_OUT_OF_SPACE;

      uint32_t uinteger;
      memcpy(&uinteger, (char*)instance + def[i].offset, sizeof(uint32_t));
      uint32_t net_uinteger = htonl(uinteger);
      memcpy((char*)out + offset, &net_uinteger, sizeof(uint32_t));

      offset += sizeof(uint32_t);
      break;
    case MICRO_SERDE_STR:;
      if (offset + def[i].size >= out_size)
        return MICRO_SERDE_ERROR_OUT_OF_SPACE;

      memcpy(out + offset, (char*)instance + def[i].offset, def[i].size);
      offset += def[i].size;      
      break;
    case MICRO_SERDE_CUSTOM:
      if (!def[i].custom_def)
        return MICRO_SERDE_ERROR_MISSING_CUSTOM_DEF;

      int ret = micro_serde_serialize(def[i].custom_def,
                                      def[i].size,
                                      (char*)instance + def[i].offset,
                                      out + offset,
                                      out_size - offset);
      if (ret < 0) return ret;
      offset += ret;
      break;
    default:
      return MICRO_SERDE_ERROR_UNKNOWN_TYPE;
    }
  }
  
  return offset;
}

MICRO_SERDE_DEF int
micro_serde_deserialize(MicroSerdeDef *def,
                        unsigned int field_count,
                        void* instance,
                        const char *input,
                        unsigned int input_size)
{
  if (!def) return MICRO_SERDE_ERROR_DEF_NULL;
  if (!instance || !input) return MICRO_SERDE_ERROR_ARG_NULL;

  unsigned int offset = 0;
  for (unsigned int i = 0; i < field_count; ++i)
  {
    switch(def[i].type)
    {
    case MICRO_SERDE_INT: // fall
    case MICRO_SERDE_UINT:
      if (offset + def[i].size >= input_size)
        return MICRO_SERDE_ERROR_OUT_OF_SPACE;

      uint32_t net_uinteger;
      memcpy(&net_uinteger, input + offset, sizeof(uint32_t));
      uint32_t uinteger = ntohl(net_uinteger);
      memcpy((char*)instance + def[i].offset, &uinteger, sizeof(uint32_t));

      offset += def[i].size;
      break;
    case MICRO_SERDE_STR:;
      if (offset + def[i].size >= input_size)
        return MICRO_SERDE_ERROR_OUT_OF_SPACE;

      memcpy((char*)instance + def[i].offset, input + offset, def[i].size);
      offset += def[i].size;
      break;
    case MICRO_SERDE_CUSTOM:
      if (!def[i].custom_def)
        return MICRO_SERDE_ERROR_MISSING_CUSTOM_DEF;

      int ret = micro_serde_deserialize(def[i].custom_def,
                                        def[i].size,
                                        (char*)instance + def[i].offset,
                                        input + offset,
                                        input_size - offset);
      if (ret < 0) return ret;
      offset += ret;
      break;
    default:
      return MICRO_SERDE_ERROR_UNKNOWN_TYPE;
    }
  }
  
  return offset;
}

#endif // MICRO_SERDE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // MICRO_SERDE
