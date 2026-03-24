//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// llist.h
// =======
//
// Implementation of a type-safe, generic doubly-linked list in C99.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Documentation
// -------------
//
// This header provides a type-safe, generic doubly-linked list
// implementation in C99, fully usable as a header-only library with
// no dependencies.
//
// The list supports:
//
//   - O(1) push and pop at both the head and tail.
//   - Iteration over elements via head/tail pointers.
//   - Automatic tracking of list length.
//   - Customizable value types and user specified per-element free function.
//
// Api:
//
//    LLIST_DECLARE(prefix, type, free_fn)
//        Declare a linked list for [type]
//        Args:
//          - prefix: define a prefix for the linked list functions and types.
//          - type: the type of values stored in the linked list
//          - free_fn: a function that will be called when a value gets deleted.
//            It should have the following signature:
//
//            void free_fn(type* value);
//
//    prefix_llist
//        The linked list type
//
//    int prefix_llist_init(prefix_llist *list)
//        Initialized a linked list with default values
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Remember to free the list when you are done with prefix_llist_free
//
//    int prefix_llist_free(prefix_llist *list)
//        Free all allocated memory from the list
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Time complexity: O(len)
//    
//    int prefix_llist_push_front(prefix_llist *list, type value)
//        Adds [value] at the head of [list]
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Time complexity: O(1)
//
//    int prefix_llist_push_back(prefix_llist *list, type value)
//        Adds [value] at the tail of [list]
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Time complexity: O(1)
//
//    int prefix_llist_pop_front(prefix_llist *list)
//        Removes the head value from [list]
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Time complexity: O(1)
//
//    int prefix_llist_pop_back(prefix_llist *list)
//        Removes the tail value from [list]
//        Returns 0 on success, or a negative LLIST_ERROR_ value otherwise.
//        Time complexity: O(1)
//
//    value* prefix_llist_head(prefix_llist *list)
//        Returns a pointer to the head value from [list]
//        In case of failure or if no head is found, returns NULL
//        Time complexity: O(1)
//
//    value* prefix_llist_tail(prefix_llist *list)
//        Returns a pointer to the tail value from [list]
//        In case of failure or if no tail is found, returns NULL
//        Time complexity: O(1)
//
//    int prefix_llist_len(prefix_llist *list)
//        Returns the length of [list]
//        In case of failure returns a negative LLIST_ERROR_
//        Time complexity: O(1)
//
//
// Usage
// ----
//
// Just #include "llist.h".
//
// You can tune the library by #defining the allocator and free
// functions. See the "Config" comments under "Configuration" below.
//
// To create a linked list for a certain type you first need to
// declare it using LLIST_DECLARE. You need to specify respectively
// the prefix, the type, and the function to call when a value gets
// deleted.  For example, the following code creates the type `Test`
// and declares a linked list for this type:
//
//    typedef int Test;  // This will be the value type stored in the list
//    // Define a free function for the type in the list
//    void test_free(Test* val) { printf("Value free\n"); return; }
//    // Declare the list
//    LLIST_DECLARE(test, Test, test_free)
//
// Finally you can start using the list_* functions prefixed with your
// specified name. Here is an example:
//
//    int main(void)
//    {
//       test_llist list;
//       test_llist_init(&list);
//
//       test_llist_push_front(&list, 123);
//       assert(test_llist_len(&list) == 1);
//       assert(*test_llist_head(&list) == 123);
//       assert(*test_llist_tail(&list) == 123);
//
//       test_llist_free(&list);
//       return 0;
//    }
//
//
// Code
// ----
//
// The official git repository of llist.h is hosted at:
//
//     https://github.com/San7o/llist.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//

#ifndef LLIST
#define LLIST

#define LLIST_MAJOR 0
#define LLIST_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Configuration
//

// Config: Define memory allocation function
// Notes: This is expected to be used like malloc(3)
#ifndef LLIST_MALLOC
  #include <stdlib.h>
  #define LLIST_MALLOC malloc
#endif

// Config: Define memory free function
// Notes: This is expected to be used like free(3)
#ifndef LLIST_FREE
  #include <stdlib.h>
  #define LLIST_FREE free
#endif

// Config: Prefix for all functions
#ifndef LLIST_DEF
  #define LLIST_DEF static inline
#endif
  
//
// Types
//

#define LLIST_OK                0
#define LLIST_ERROR_LIST_NULL  -1
#define LLIST_ERROR_ALLOCATION -2
#define LLIST_ERROR_HEAD_TAIL_MISMATCH -3

#define LLIST_DECLARE(prefix,                                         \
                      value_type,                                     \
                      value_free_fn)                                  \
                                                                      \
  struct prefix##_llist_elem;                                         \
  typedef struct prefix##_llist_elem prefix##_llist_elem;             \
                                                                      \
  struct prefix##_llist_elem {                                        \
    prefix##_llist_elem *next;                                        \
    prefix##_llist_elem *prev;                                        \
    value_type value;                                                 \
  };                                                                  \
                                                                      \
  typedef struct {                                                    \
    prefix##_llist_elem *head;                                        \
    prefix##_llist_elem *tail;                                        \
    unsigned int len;                                                 \
  } prefix##_llist;                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_init(prefix##_llist *list)             \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
                                                                      \
    list->len = 0;                                                    \
    list->head = NULL;                                                \
    list->tail = NULL;                                                \
                                                                      \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_free(prefix##_llist *list)             \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
    if (!list->head) return LLIST_OK;                                 \
                                                                      \
    prefix##_llist_elem* it = list->head;                             \
    while(it)                                                         \
    {                                                                 \
      prefix##_llist_elem* next = it->next;                           \
      value_free_fn(&it->value);                                      \
      LLIST_FREE(it);                                                 \
      it = next;                                                      \
      list->len--;                                                    \
    }                                                                 \
                                                                      \
    list->head = NULL;                                                \
    list->tail = NULL;                                                \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_push_front(prefix##_llist *list,       \
                                              value_type value)       \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
                                                                      \
    prefix##_llist_elem *new_elem =                                   \
      LLIST_MALLOC(sizeof(prefix##_llist_elem));                      \
    if (!new_elem) return LLIST_ERROR_ALLOCATION;                     \
                                                                      \
    if (!list->head)                                                  \
    {                                                                 \
      new_elem->next = NULL;                                          \
      new_elem->prev = NULL;                                          \
      new_elem->value = value;                                        \
      list->tail = new_elem;                                          \
      list->head = new_elem;                                          \
      list->len++;                                                    \
      return LLIST_OK;                                                \
    }                                                                 \
                                                                      \
    new_elem->next = list->head;                                      \
    new_elem->prev = NULL;                                            \
    new_elem->value = value;                                          \
    list->head->prev = new_elem;                                      \
    list->head = new_elem;                                            \
    list->len++;                                                      \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_push_back(prefix##_llist *list,        \
                                             value_type value)        \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
                                                                      \
    prefix##_llist_elem *new_elem =                                   \
      LLIST_MALLOC(sizeof(prefix##_llist_elem));                      \
    if (!new_elem) return LLIST_ERROR_ALLOCATION;                     \
                                                                      \
    if (!list->tail)                                                  \
    {                                                                 \
      new_elem->next = NULL;                                          \
      new_elem->prev = NULL;                                          \
      new_elem->value = value;                                        \
      list->tail = new_elem;                                          \
      list->head = new_elem;                                          \
      list->len++;                                                    \
      return LLIST_OK;                                                \
    }                                                                 \
                                                                      \
    new_elem->next = NULL;                                            \
    new_elem->prev = list->tail;                                      \
    new_elem->value = value;                                          \
    list->tail->next = new_elem;                                      \
    list->tail = new_elem;                                            \
    list->len++;                                                      \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_pop_front(prefix##_llist *list)        \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
    if (!list->head) return LLIST_OK;                                 \
                                                                      \
    if (list->head->next == NULL)                                     \
    {                                                                 \
      if (list->head != list->tail)                                   \
        return LLIST_ERROR_HEAD_TAIL_MISMATCH;                        \
                                                                      \
      value_free_fn(&list->head->value);                              \
      LLIST_FREE(list->head);                                         \
      list->head = NULL;                                              \
      list->tail = NULL;                                              \
      list->len--;                                                    \
      return LLIST_OK;                                                \
    }                                                                 \
                                                                      \
    prefix##_llist_elem* head = list->head;                           \
    list->head->next->prev = NULL;                                    \
    list->head = list->head->next;                                    \
    value_free_fn(&head->value);                                      \
    LLIST_FREE(head);                                                 \
    list->len--;                                                      \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_pop_back(prefix##_llist *list)         \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
    if (!list->tail) return LLIST_OK;                                 \
                                                                      \
    if (list->tail->prev == NULL)                                     \
    {                                                                 \
      if (list->head != list->tail)                                   \
        return LLIST_ERROR_HEAD_TAIL_MISMATCH;                        \
                                                                      \
      value_free_fn(&list->tail->value);                              \
      LLIST_FREE(list->tail);                                         \
      list->head = NULL;                                              \
      list->tail = NULL;                                              \
      list->len--;                                                    \
      return LLIST_OK;                                                \
    }                                                                 \
                                                                      \
    prefix##_llist_elem *tail = list->tail;                           \
    list->tail->prev->next = NULL;                                    \
    list->tail = list->tail->prev;                                    \
    value_free_fn(&tail->value);                                      \
    LLIST_FREE(tail);                                                 \
    list->len--;                                                      \
    return LLIST_OK;                                                  \
  }                                                                   \
                                                                      \
  LLIST_DEF value_type* prefix##_llist_head(prefix##_llist *list)     \
  {                                                                   \
    if (!list || !list->head) return NULL;                            \
    return &list->head->value;                                        \
  }                                                                   \
                                                                      \
  LLIST_DEF value_type* prefix##_llist_tail(prefix##_llist *list)     \
  {                                                                   \
    if (!list || !list->tail) return NULL;                            \
    return &list->tail->value;                                        \
  }                                                                   \
                                                                      \
  LLIST_DEF int prefix##_llist_len(prefix##_llist *list)              \
  {                                                                   \
    if (!list) return LLIST_ERROR_LIST_NULL;                          \
    return list->len;                                                 \
  }

//
// Example
//

#if 0

#include "llist.h"

#include <stdio.h>
#include <assert.h>

typedef int Test;
void test_free(Test* val) { printf("Value free\n"); return; }
LLIST_DECLARE(test, Test, test_free)
  
int main(void)
{
  test_llist list;
  test_llist_init(&list);

  test_llist_push_back(&list, 6969);
  
  assert(test_llist_len(&list) == 1);
  assert(*test_llist_head(&list) == 6969);
  assert(*test_llist_tail(&list) == 6969);

  test_llist_push_back(&list, 420);
  
  assert(test_llist_len(&list) == 2);
  assert(*test_llist_head(&list) == 6969);
  assert(*test_llist_tail(&list) == 420);
  
  test_llist_free(&list);
  return 0;
}

  
#endif //0
  
#ifdef __cplusplus
}
#endif
  
#endif // LLIST
