//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// hashset.h
// =========
//
// Header-only implementation of an hashset in C99 for any type.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Documentation
// -------------
//
// A set is a data structure where each element occurs only once. This
// header implements a set using an hashmap with tombstones.
//
// Api:
//
//    HASHSET_DECLARE(prefix, type, hash_fn, eq_fn)
//        Declare a new hashset for [type]
//        Args:
//          - prefix: prefix to all the hashset functions and types
//          - type: the value type stored in the set
//          - hash_fn: hash function for [type]. Must have the signature:
//
//                hashset_hash_t hash_fn(type val, unsigned int val_len);
//
//          - eq_fn: function to compare two [type] values, with signature:
//
//                bool eq_fn(type a, unsigned int a_len,
//                           type b, unsigned int b_len);
//
//    prefix_set
//        The hashset type
//
//    int prefix_set_init(prefix_set *set);
//        Initializes [set]
//        Returns: 0 on success, or a negative integer on error.
//        Notes: Remember to destroy the set when you are done.
//
//    void prefix_set_destroy(prefix_set *set);
//        Destroys [set]
//
//    size_t prefix_set_find_slot(prefix_set *set,
//                                type key,
//                                unsigned int key_len);
//        Finds the next slot available for [key] of [ley_len] length
//        in [map]
//        Returns: a non-negative position in the internal map of the
//        closest free slot, or a negative integer on error.
//
//    int prefix_set_resize(prefix_set *set,
//                          size_t newcap);
//        Resizes [set] with [newcap] capacity.
//        Returns: 0 on success, or a negative integer on error.
//
//    bool prefix_set_insert(prefix_set *set,
//                           type key,
//                           unsigned int key_len);
//         Insert [key] element of [key_len] length in [set]
//         Returns: true if insertion succeed, or false otherwise.
//
//    bool prefix_set_contains(prefix_set *set,
//                             type key,
//                             unsigned int key_len);
//          Check if [set] contains [key] of [key_len] length
//          Returns: true if [set] contains the key, or false otherwise.
//
//    bool prefix_set_remove(prefix_set *set,
//                           type key,
//                           unsigned int key_len);
//          Removes [key] of [key_len] length from [set]
//          Returns: true if the key was successfully removed, or false
//          otherwise.
//
//
// Usage
// -----
//
// Just #include "hashset.h".
//
// If you want to use the built-in hash functions, you need to also
//
//    #define HASHSET_IMPLEMENTATION
//
// before including the header.
//
// You can tune the library by #defining the allocator and free
// functions. See the "Config" comments under "Configuration" below.
//
// To start using an hashmap, you first need to declare one with the
// macro HASHSET_DECLARE. For example:
//
// 
//    bool eq_u32(uint32_t a, unsigned int a_size,
//                uint32_t b, unsigned int b_size)
//    { return a == b; }
//
//    #define HASHSET_DECLARE(u32, uint32_t, hashset_hash_int32,
//                            eq_u32)
//
// Now you can start using the functions prefixed with "example".
//
//    u32_set s;
//    u32_set_init(&s);
//
//    u32_set_insert(&s, 42, 0);
//    assert(u32_set_contains(&s, 42, 0) == 1);
//
//    u32_set_destroy(&s);         
//
// See full example at the end of the header.
//
//
// Code
// ----
//
// The official git repository of hashset.h is hosted at:
//
//     https://github.com/San7o/hashset.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//


#ifndef HASHSET
#define HASHSET

#define HASHSET_MAJOR 0
#define HASHSET_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

//
// Configuration
//

// Config: The initial capacity of the hashset
#ifndef HASHSET_INITIAL_CAPACITY
  #define HASHSET_INITIAL_CAPACITY 16
#endif

// Config: the maximum load factor before resizing
#ifndef HASHSET_MAX_LOAD_FACTOR
  #define HASHSET_MAX_LOAD_FACTOR 0.7
#endif

// Config: The type of an hash
#ifndef HASHSET_HASH_T
  #define HASHSET_HASH_T unsigned int
#endif

// Config: The memory allocator
// Note: This is expected to be used like calloc(3), zero the memory
#ifndef HASHSET_CALLOC
  #ifdef MICRO_HEADERS_CALLOC
    #define HASHSET_CALLOC MICRO_HEADERS_CALLOC
  #else
    #include <stdlib.h>
    #define HASHSET_CALLOC calloc
  #endif
#endif

// Config: Function to free the memory
#ifndef HASHSET_FREE
  #ifdef MICRO_HEADERS_FREE
    #define HASHSET_FREE MICRO_HEADERS_FREE
  #else
    #include <stdlib.h>
    #define HASHSET_FREE free
  #endif
#endif

// Config: Prefix for all functions
#ifndef HASHSET_DEF
  #define HASHSET_DEF static inline
#endif
  
typedef HASHSET_HASH_T hashset_hash_t;

//
// Macros
//

#define HASHSET_OK                0
#define HASHSET_ERROR_SET_NULL   -1
#define HASHSET_ERROR_ALLOCATION -2
  
#define HASHSET_DECLARE(prefix, type, hash_fn, eq_fn)                   \
  typedef struct {                                                      \
    type val;                                                           \
    unsigned int size;                                                  \
  } prefix##_##type##_size_pair;                                        \
                                                                        \
  typedef struct {                                                      \
    prefix##_##type##_size_pair *data;                                  \
    uint8_t *state; /* 0=empty,1=used,2=deleted */                      \
    size_t size;                                                        \
    size_t capacity;                                                    \
  } prefix##_set;                                                       \
                                                                        \
  HASHSET_DEF int prefix##_set_init(prefix##_set *set)                  \
  {                                                                     \
    if (!set) return HASHSET_ERROR_SET_NULL;                            \
                                                                        \
    set->size = 0;                                                      \
    set->capacity = HASHSET_INITIAL_CAPACITY;                           \
    set->data = HASHSET_CALLOC(set->capacity,                           \
                               sizeof(prefix##_##type##_size_pair));    \
    if (!set->data) return HASHSET_ERROR_ALLOCATION;                    \
    set->state = HASHSET_CALLOC(set->capacity, sizeof(uint8_t));        \
    if (!set->state) return HASHSET_ERROR_ALLOCATION;                   \
                                                                        \
    return HASHSET_OK;                                                  \
  }                                                                     \
                                                                        \
  HASHSET_DEF void prefix##_set_destroy(prefix##_set *set)              \
  {                                                                     \
    if (!set) return;                                                   \
                                                                        \
    if (set->data)                                                      \
      HASHSET_FREE(set->data);                                          \
    if (set->state)                                                     \
      HASHSET_FREE(set->state);                                         \
    set->data = NULL; set->state = NULL;                                \
    set->size = set->capacity = 0;                                      \
                                                                        \
    return;                                                             \
  }                                                                     \
                                                                        \
  HASHSET_DEF size_t prefix##_set_find_slot(prefix##_set *set,          \
                                              type key,                 \
                                              unsigned int key_len)     \
  {                                                                     \
    if (!set) return HASHSET_ERROR_SET_NULL;                            \
                                                                        \
    size_t mask = set->capacity - 1;                                    \
    hashset_hash_t idx = hash_fn(key, key_len) & mask;                  \
    size_t start = idx;                                                 \
    while (set->state[idx] == 1                                         \
           && !eq_fn(set->data[idx].val, set->data[idx].size,           \
                     key, key_len))                                     \
    {                                                                   \
      idx = (idx + 1) & mask;                                           \
      if (idx == start) return set->capacity; /* full */                \
    }                                                                   \
    return idx;                                                         \
  }                                                                     \
                                                                        \
  HASHSET_DEF int prefix##_set_resize(prefix##_set *set,                \
                                        size_t newcap)                  \
  {                                                                     \
    if (!set) return HASHSET_ERROR_SET_NULL;                            \
                                                                        \
    prefix##_##type##_size_pair *old_data = set->data;                  \
    uint8_t *old_state = set->state;                                    \
    size_t old_cap = set->capacity;                                     \
                                                                        \
    set->capacity = newcap;                                             \
    set->data = HASHSET_CALLOC(newcap,                                  \
                               sizeof(prefix##_##type##_size_pair));    \
    if (!set->data) return HASHSET_ERROR_ALLOCATION;                    \
    set->state = HASHSET_CALLOC(newcap, sizeof(uint8_t));               \
    if (!set->state) return HASHSET_ERROR_ALLOCATION;                   \
    set->size = 0;                                                      \
                                                                        \
    for (size_t i = 0; i < old_cap; i++)                                \
    {                                                                   \
      if (old_state[i] == 1)                                            \
      {                                                                 \
        prefix##_##type##_size_pair val = old_data[i];                  \
        size_t slot = prefix##_set_find_slot(set, val.val, val.size);   \
        if (slot < 0) continue;                                         \
        set->data[slot] = val;                                          \
        set->state[slot] = 1;                                           \
        set->size++;                                                    \
      }                                                                 \
    }                                                                   \
                                                                        \
    HASHSET_FREE(old_data);                                             \
    HASHSET_FREE(old_state);                                            \
    return HASHSET_OK;                                                  \
  }                                                                     \
                                                                        \
  HASHSET_DEF bool prefix##_set_insert(prefix##_set *set,               \
                                         type key,                      \
                                         unsigned int key_len)          \
  {                                                                     \
    if (set == NULL) return false;                                      \
    if ((double)set->size / set->capacity > HASHSET_MAX_LOAD_FACTOR)    \
      prefix##_set_resize(set, set->capacity * 2);                      \
                                                                        \
    size_t idx = prefix##_set_find_slot(set, key, key_len);             \
    if (idx< 0) return false;                                           \
                                                                        \
    if (set->state[idx] == 1) return false; /* already exists */        \
    set->data[idx] =                                                    \
      (prefix##_##type##_size_pair) {.val = key, .size = key_len};      \
    set->state[idx] = 1;                                                \
    set->size++;                                                        \
                                                                        \
    return true;                                                        \
  }                                                                     \
                                                                        \
  HASHSET_DEF bool prefix##_set_contains(prefix##_set *set,             \
                                           type key,                    \
                                           unsigned int key_len)        \
  {                                                                     \
    if (!set) return false;                                             \
    size_t idx = prefix##_set_find_slot(set, key, key_len);             \
    if (idx < 0) return false;                                          \
    return set->state[idx] == 1;                                        \
  }                                                                     \
                                                                        \
  HASHSET_DEF bool prefix##_set_remove(prefix##_set *set,               \
                                         type key,                      \
                                         unsigned int key_len)          \
  {                                                                     \
    if (!set) return false;                                             \
    size_t idx = prefix##_set_find_slot(set, key, key_len);             \
    if (set->state[idx] != 1) return false;                             \
    set->state[idx] = 2; /* mark deleted */                             \
    set->size--;                                                        \
    return true;                                                        \
  }

//
// Function Declarations
//

// Some sample hash functions
// You need to #define HASHSET_IMPLEMENTATION to use them.
  
// char* key hash function
// Credits to http://www.cse.yorku.ca/~oz/hash.html
HASHSET_DEF unsigned long hashset_hash_char(char *bytes, unsigned int len);

// uint32_t key hash function
HASHSET_DEF uint32_t hashset_hash_int32(uint32_t a, unsigned int ignored);
  
//
// Implementation
//

#ifdef HASHSET_IMPLEMENTATION

HASHSET_DEF unsigned long hashset_hash_char(char *bytes, unsigned int len)
{
  unsigned long hash = 5381;
  for (unsigned int i = 0; i < len; ++i)
    hash = hash * 33 + bytes[i];
  return hash;
}

HASHSET_DEF uint32_t hashset_hash_int32(uint32_t a, unsigned int ignored)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

#endif // HASHSET_IMPLEMENTATION

//
// Examples
//

#if 0

#define HASHSET_IMPLEMENTATION
#include "hashset.h"

#include <stdio.h>
#include <assert.h>

bool eq_u32(uint32_t a, unsigned int a_size,
            uint32_t b, unsigned int b_size)
{ return a == b; }

// Declare a uint32_t hashset
HASHSET_DECLARE(u32, uint32_t, hashset_hash_int32, eq_u32)

int main(void) {
    u32_set s;
    assert(u32_set_init(&s) == 0);

    assert(u32_set_insert(&s, 42, 0) == 1);
    assert(u32_set_insert(&s, 1337, 0) == 1);
    
    assert(u32_set_contains(&s, 42, 0) == 1);
    assert(u32_set_contains(&s, 1337, 0) == 1);
    
    assert(u32_set_remove(&s, 42, 0) == 1);
    assert(u32_set_contains(&s, 42, 0) == 0);

    u32_set_destroy(&s);
    return 0;
}

#endif // 0

#ifdef __cplusplus
}
#endif

#endif // HASHSET
