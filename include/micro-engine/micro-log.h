//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-log.h
// ===========
//
// Header-only, configurable, thread safe logging framework in C99.
//
// ```
// 2025-09-21 22:32:36 INFO   | Logger initialized
// 2025-09-21 22:32:36 INFO   | I’d just like to interject for a moment...
// 2025-09-21 22:32:36 INFO   | Closing logger
// ```
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Features
// --------
//
//  - Multiple logging levels
//  - Log to stdout, file, UNIX sockets, and network sockets
//  - Configurable metadata (level, date, time, pid, tid, etc.)
//  - JSON serialization support
//  - Thread-safe logging
//  - read settings from file (see the file `settings`)
//  - optional colored output
//  - compile time settings
//
// Initial implementation based on Oak: https://github.com/San7o/oak
//
// Example
// -------
// 
//    #define MICRO_LOG_IMPLEMENTATION
//    #include "micro-log.h"
// 
//    int main(void)
//    {
//       micro_log_init();
//       micro_log_set_flags(MICRO_LOG_FLAG_LEVEL 
//                           | MICRO_LOG_FLAG_DATE  
//                           | MICRO_LOG_FLAG_TIME);
//   
//       micro_log_info("I’d just like to interject for a moment...");
//   
//       micro_log_close();  
//     }
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_LOG_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_LOG_IMPLEMENTATION
//   #include "micro-log.h"
//
// You can tune the library by #defining certain values. See the
// "Config" comments under "Configuration" below.
//
// !!IMPORTANT: READ THIS FOR MULTITHREADING !!
// Multithreading support is optional as it adds a small performance
// setback since printing must be regulated via a mutex.  If you want
// to support logging from multiple threads, then define
// MICRO_LOG_MULTITHREADED before including this file.
//
// (Almost) All log function have two versions: one that interacts
// with a global logger, and another that uses a logger instance you
// supply. This is wanted because most of the time you just want a
// global logger without the need to pass references to loggers, but
// you may also need different loggers for different parts of your
// application, hence both options are provided.
//
// We will see some examples with the global logger. To use a local
// one, you just need to use the version '2' of the functions and pass
// the logger as the first parameter. For example
// `micro_log_info(...)` becomes `micro_log_info2(MicroLog logger,
// ...)`.
//
// After including the library, you can initialize a logger with
// `micro_log_init`.
//
// ```
// micro_log_init();
// ```
//
// If using `micro_log_init2`, you can specify a pointer to a logger.
//
// Remember to close the logger after you are done with
// `micro_log_close()`.
//
// You can set additional settings like the log level with
// `micro_log_set_level` or a file with `micro_log_set_file`, check
// out the function declarations.
//
// To log something, use the macro `micro_log_` with the level you
// want to use, like:
//
// ```
// micro_log_info("I’d like to interject for a moment. %s", text);
// ```
//
// You can format the logs like printf(3).
//
// Check out more examples at the end of the header.
//
// You can also read some settings from a file. Check out the file
// `settings` for additional information.
//
//
// Code
// ----
//
// The official git repository of micro-log.h is hosted at:
//
//     https://github.com/San7o/micro-log.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//
//
// TODO
// ----
//
// - windows support
//

#ifndef MICRO_LOG
#define MICRO_LOG

#define MICRO_LOG_MAJOR 0
#define MICRO_LOG_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Configuration
//
  
// Config: Enable thread safety by defining MICRO_LOG_MULTITHREADED
//
// Note: This is optional since it adds a (tiny) performance setback
// since printing must be regulated via a mutex.
//
//#define MICRO_LOG_MULTITHREADED

// Config: Enable logging on operating system sockets
//
// Note: Optional since this may not be needed by most applications
//
//#define MICRO_LOG_SOCKETS

// Config: compiler-time log level, value 0 to 6 (included)
//
// All calls to log functions in the family `micro_log_{level}` and
// `micro_log_{level}2` with less priority than this will not be
// compiled at all. Setting this to an high priority will save
// performance since the log level will not have to be checked during
// runtime, and the log functions will not be called at all.
//
// Set this to one among the MICRO_LOG_LEVEL_* values:
//
//     MICRO_LOG_LEVEL_{TRACE|DEBUF|INFO|WARN|ERROR|FATAL|DISABLED}
//
#ifndef MICRO_LOG_LEVEL_DEF
  #define MICRO_LOG_LEVEL_DEF MICRO_LOG_LEVEL_TRACE
#endif

// Config: maximum size of a log string
#ifndef MICRO_LOG_MAX_STRING_SIZE
  #define MICRO_LOG_MAX_STRING_SIZE 1024
#endif
  
// Config: Prefix for all functions
// For function inlining, set this to `static inline` and then define
// the implementation in all the files
//
#ifndef MICRO_LOG_DEF
  #define MICRO_LOG_DEF extern
#endif

// Config: Support date and time printing (includes time.h)
#if 0
  #define MICRO_LOG_DATE_TIME
#endif
  
// Config: Support PID and TID printing (includes pthread.h)
#if 0
  #define MICRO_LOG_PID
#endif
  
// Config: Memory functions
#ifndef MICRO_LOG_MALLOC
  #ifdef MICRO_HEADERS_MALLOC
    #define MICRO_LOG_MALLOC MICRO_HEADERS_MALLOC
  #else
    #define MICRO_LOG_MALLOC malloc
  #endif
#endif

#ifndef MICRO_LOG_FREE
  #ifdef MICRO_HEADERS_FREE
    #define MICRO_LOG_FREE MICRO_HEADERS_FREE
  #else
    #define MICRO_LOG_FREE free
  #endif
#endif

#ifndef MICRO_LOG_REALLOC
  #ifdef MICRO_HEADERS_REALLOC
    #define MICRO_LOG_REALLOC MICRO_HEADERS_REALLOC
  #else
    #define MICRO_LOG_REALLOC realloc
  #endif
#endif

// I/O operations

#ifndef MICRO_LOG_FOPEN
  #ifdef MICRO_HEADERS_FOPEN
    #define MICRO_LOG_FOPEN MICRO_HEADERS_FOPEN
  #else
    #include <stdio.h>
    #define MICRO_LOG_FOPEN fopen
  #endif
  #ifndef MICRO_FILE_MODE_READ
    #define MICRO_FILE_MODE_READ "r"
  #endif
  #ifndef MICRO_FILE_MODE_WRITE
    #define MICRO_FILE_MODE_WRITE "w+"
  #endif
#endif

#ifndef MICRO_LOG_FCLOSE
  #ifdef MICRO_HEADERS_FCLOSE
    #define MICRO_LOG_FCLOSE MICRO_HEADERS_FCLOSE
  #else
    #define MICRO_LOG_FCLOSE fclose
  #endif
#endif

#ifndef MICRO_LOG_FREAD
  #ifdef MICRO_HEADERS_FREAD
    #define MICRO_LOG_FREAD MICRO_HEADERS_FREAD
  #else
    #define MICRO_LOG_FREAD fread
  #endif
#endif
 
#ifndef MICRO_LOG_FWRITE
  #ifdef MICRO_HEADERS_FWRITE
    #define MICRO_LOG_FWRITE MICRO_HEADERS_FWRITE
  #else
    #define MICRO_LOG_FWRITE fwrite
  #endif
#endif

#ifndef MICRO_LOG_FLUSH
  #ifdef MICRO_HEADERS_FLUSH
    #define MICRO_LOG_FLUSH MICRO_HEADERS_FLUSH
  #else
    #define MICRO_LOG_FLUSH(...) true
  #endif
#endif

#ifndef MICRO_LOG_OUT
  #ifdef MICRO_HEADERS_OUT
    #define MICRO_LOG_OUT MICRO_HEADERS_OUT
  #else
    #define MICRO_LOG_OUT printf
  #endif
#endif
  
//
// Errors
//

// Do not expect the error numbers to be the same in the future, use
// the macro
#define MICRO_LOG_OK                         0
#define MICRO_LOG_ERROR_LOGGER_NULL          1
#define MICRO_LOG_ERROR_UNIMPLEMENTED        2
#define MICRO_LOG_ERROR_MUTEX_LOCK           3
#define MICRO_LOG_ERROR_MUTEX_UNLOCK         4
#define MICRO_LOG_ERROR_CLOSE_FILE           5
#define MICRO_LOG_ERROR_CLOSE_INET_SOCK      6
#define MICRO_LOG_ERROR_CLOSE_UNIX_SOCK      7
#define MICRO_LOG_ERROR_OPEN_FILE            8
#define MICRO_LOG_ERROR_INET_ADDR_NULL       9
#define MICRO_LOG_ERROR_OPEN_INET_SOCK       10
#define MICRO_LOG_ERROR_INET_CONNECT         11
#define MICRO_LOG_ERROR_PRINTF_SOCK_INET     12
#define MICRO_LOG_ERROR_VPRINTF_SOCK_INET    13
#define MICRO_LOG_ERROR_PRINTF_SOCK_UNIX     14
#define MICRO_LOG_ERROR_VPRINTF_SOCK_UNIX    15
#define MICRO_LOG_ERROR_PRINTF_STDOUT        16
#define MICRO_LOG_ERROR_PRINTF_FILE          17
#define MICRO_LOG_ERROR_VPRINTF_STDOUT       18
#define MICRO_LOG_ERROR_VPRINTF_FILE         19
#define MICRO_LOG_ERROR_INET_ADDR            21
#define MICRO_LOG_ERROR_SOCK_PATH_NULL       22
#define MICRO_LOG_ERROR_OPEN_UNIX_SOCK       23
#define MICRO_LOG_ERROR_UNIX_CONNECT         24
#define MICRO_LOG_ERROR_FLUSH_STDOUT         25
#define MICRO_LOG_ERROR_FLUSH_FILE           26
#define MICRO_LOG_ERROR_FROM_FILE_NULL       27
#define MICRO_LOG_ERROR_INVALID_FILE_SETTING 28
#define MICRO_LOG_ERROR_UNKNOWN_LEVEL        29
#define MICRO_LOG_ERROR_UNKNOWN_FLAG         30
#define MICRO_LOG_ERROR_INVALID_INET_ADDR    31
#define MICRO_LOG_ERROR_INVALID_PORT         32
#define MICRO_LOG_ERROR_INVALID_PROTOCOL     33
#define _MICRO_LOG_ERROR_MAX                 34

//
// Macros
//

#ifdef MICRO_LOG_MULTITHREADED
  #ifdef _WIN32
    #error "TODO: Multithreading support on windows is not yet supported"
  #endif
  #include <pthread.h>
#endif
  
#ifndef _SIZE_T_DEFINED
  #define _SIZE_T_DEFINED
  typedef __SIZE_TYPE__ size_t;
#endif
#ifndef true
  #define true  1
#endif
#ifndef false
  #define false 0
#endif
#ifndef bool
  #define bool _Bool;
#endif
#ifndef NULL
  #define NULL ((void*) 0)
#endif

typedef __builtin_va_list _micro_log_va_list;

#define MICRO_LOG_FLAG_NONE  (0)
#define MICRO_LOG_FLAG_LEVEL (1 << 0)
#define MICRO_LOG_FLAG_DATE  (1 << 1)
#define MICRO_LOG_FLAG_TIME  (1 << 2)
#define MICRO_LOG_FLAG_PID   (1 << 3)
#define MICRO_LOG_FLAG_TID   (1 << 4)
#define MICRO_LOG_FLAG_JSON  (1 << 5)
#define MICRO_LOG_FLAG_COLOR (1 << 6)
#define MICRO_LOG_FLAG_FILE  (1 << 7)
#define MICRO_LOG_FLAG_LINE  (1 << 8)

#define MICRO_LOG_OUT_STDOUT    (1 << 0)
#define MICRO_LOG_OUT_FILE      (1 << 1)
#ifdef MICRO_LOG_SOCKETS
#define MICRO_LOG_OUT_SOCK_INET (1 << 2)
#if defined(__unix__) || defined(__unix)
#define MICRO_LOG_OUT_SOCK_UNIX (1 << 3)
#endif // __unix__
#endif // MICRO_LOG_SOCKETS
#define _MICRO_LOG_OUT_MAX      (1 << 4)

#define MICRO_LOG_RST  "\x1B[0m"
#define MICRO_LOG_RED(x) "\x1B[31m" x MICRO_LOG_RST
#define MICRO_LOG_GRN(x) "\x1B[32m" x MICRO_LOG_RST
#define MICRO_LOG_YEL(x) "\x1B[33m" x MICRO_LOG_RST
#define MICRO_LOG_BLU(x) "\x1B[34m" x MICRO_LOG_RST
#define MICRO_LOG_MAG(x) "\x1B[35m" x MICRO_LOG_RST
#define MICRO_LOG_CYN(x) "\x1B[36m" x MICRO_LOG_RST
#define MICRO_LOG_WHT(x) "\x1B[37m" x MICRO_LOG_RST
#define MICRO_LOG_BOLD(x) "\x1B[1m" x MICRO_LOG_RST
#define MICRO_LOG_UNDL(x) "\x1B[4m" x MICRO_LOG_RST
#define MICRO_LOG_LGRAY(x) "\x1B[90m" x MICRO_LOG_RST

// Global logger

// Functions
// micro_log_{write|trace|debug|info|warn|error|fatal}
  
#define micro_log_write(log_level, ...)                       \
  micro_log_write2(&micro_log_global, log_level, __VA_ARGS__)

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_TRACE
  #define micro_log_trace(...)                          \
    micro_log_write(MICRO_LOG_LEVEL_TRACE, __VA_ARGS__)
#else
  #define micro_log_trace(...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_DEBUG
  #define micro_log_debug(...)                          \
    micro_log_write(MICRO_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
  #define micro_log_debug(...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_INFO
  #define micro_log_info(...)                           \
    micro_log_write(MICRO_LOG_LEVEL_INFO, __VA_ARGS__)
#else
  #define micro_log_info(...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_WARN
  #define micro_log_warn(...)                           \
    micro_log_write(MICRO_LOG_LEVEL_WARN, __VA_ARGS__)
#else
  #define micro_log_warn(...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_ERROR
  #define micro_log_error(...)                          \
    micro_log_write(MICRO_LOG_LEVEL_ERROR, __VA_ARGS__)
#else
  #define micro_log_error(...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_FATAL
  #define micro_log_fatal(...)                          \
    micro_log_write(MICRO_LOG_LEVEL_FATAL, __VA_ARGS__)
#else
  #define micro_log_fatal(...) micro_log_disabled()
#endif
  
// Local logger

// Functions
// micro_log_{write2|trace2|debug2|info2|warn2|error2|fatal2}

#define micro_log_write2(micro_log, log_level, ...)                     \
  _micro_log_write_impl(micro_log, log_level, __FILE__, __LINE__, __VA_ARGS__)

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_TRACE
  #define micro_log_trace2(micro_log, ...)                          \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_TRACE, __VA_ARGS__)
#else
  #define micro_log_trace2(micro_log, ...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_DEBUG
  #define micro_log_debug2(micro_log, ...)                          \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
  #define micro_log_debug2(micro_log, ...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_INFO
  #define micro_log_info2(micro_log, ...)                           \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_INFO, __VA_ARGS__)
#else
  #define micro_log_info2(micro_log, ...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_WARN
  #define micro_log_warn2(micro_log, ...)                           \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_WARN, __VA_ARGS__)
#else
  #define micro_log_warn2(micro_log, ...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_ERROR
  #define micro_log_error2(micro_log, ...)                          \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_ERROR, __VA_ARGS__)
#else
  #define micro_log_error2(micro_log, ...) micro_log_disabled()
#endif

#if MICRO_LOG_LEVEL_DEF <= MICRO_LOG_LEVEL_FATAL
  #define micro_log_fatal2(micro_log, ...)                          \
    micro_log_write2(micro_log, MICRO_LOG_LEVEL_FATAL, __VA_ARGS__)
#else
  #define micro_log_fatal2(micro_log, ...) micro_log_disabled()
#endif
  
//
// Types and functions
//

// Use the MICRO_LOG_LEVEL_ macros for this value
typedef unsigned int MicroLogLevel;
#define MICRO_LOG_LEVEL_TRACE     0
#define MICRO_LOG_LEVEL_DEBUG     1
#define MICRO_LOG_LEVEL_INFO      2
#define MICRO_LOG_LEVEL_WARN      3
#define MICRO_LOG_LEVEL_ERROR     4
#define MICRO_LOG_LEVEL_FATAL     5
#define MICRO_LOG_LEVEL_DISABLED  6
#define MICRO_LOG_LEVEL_MAX       7


#ifdef MICRO_LOG_SOCKETS
typedef enum
{
  MICRO_LOG_PROTO_TCP = 0,
  MICRO_LOG_PROTO_UDP,
  _MICRO_LOG_PROTO_MAX
} MicroLogProto;
#endif // MICRO_LOG_SOCKETS

typedef int micro_log_error;

// The MicroLog logger
typedef struct {
  // MICRO_LOG_FLAG bitfield
  // Adds additionally log information based on the flags
  long unsigned int flags_bitfield;
  // MICRO_LOG_OUT bitfield
  // Default value is MICRO_LOG_OUT_STDOUT
  long unsigned int out_bitfield;
  // The current log level of the logger
  // Only logs that are of an higher or equal priority than this will
  // be logged.
  // Default value is MICRO_LOG_LEVEL_TRACE
  MicroLogLevel log_level;
  // (optional) Pointer to output file
  void *file;
  #ifdef MICRO_LOG_SOCKETS
  // (optional) Socket file descriptor
  int inet_sock_fd;
  #if defined(__unix__) || defined(__unix)
  int unix_sock_fd;
  #endif // __unix__
  #endif // MICRO_LOG_SOCKETS
  #ifdef MICRO_LOG_MULTITHREADED
  // Mutex to protect all writes
  pthread_mutex_t write_mutex;
  #endif // MICRO_LOG_MULTITHREADED
} MicroLog;

// Format string to [out] buffer (does not allocate)
MICRO_LOG_DEF void micro_log_format(char* out, const char* fmt, ...);
  
//
// Global logger functions
//
//
// The Global logger
//
// This is a global logger that can be used with the function below.
// You can also create other loggers, check out the other functions
// in the `Local logger functions` group.
extern MicroLog micro_log_global;
  
// Initialize the global logger
//
// Notes: remember to close it when you are done
MICRO_LOG_DEF micro_log_error micro_log_init(void);

// Read settings from file
//
// Check out the file named `settings` for additional information
// Notes: this expects the global logger to be already initialzied
MICRO_LOG_DEF micro_log_error micro_log_from_file(char *filename);

// Close the global logger
MICRO_LOG_DEF micro_log_error micro_log_close(void);

// Flush all current output streams in the global logger
MICRO_LOG_DEF micro_log_error micro_log_flush(void);

// Set output flags to global logger
//
// These are additional information to the log output, such as the
// date, line, file position etc. You can also set the output to be
// colored or to be serialized in some format like json.
//
// Check out the MICRO_LOG_FLAG_ values to see all the flags available
MICRO_LOG_DEF micro_log_error
micro_log_set_flags(long unsigned int flags_bitfield);

// Set the log level to global logger
//
// Only the logs with an higher level of the current log level will be
// printed.
//
// Check out the MicroLogLevel for a list of the supported levels.
MICRO_LOG_DEF micro_log_error micro_log_set_level(MicroLogLevel level);

// Set the output streams to the global logger
//
// You can toggle some output streams. Check out the MICRO_LOG_OUT_
// values. Remember that if you manually enable the
// MICRO_LOG_OUT_FILE, the logger will try to write to a file so you
// should have already opened it with `miro_log_set_file`. Likewise
// for the other options. By default, when you add a new output
// stream, this will be automatically registered.
MICRO_LOG_DEF micro_log_error micro_log_set_out(int out_flags);

// Set output file of the global logger
//
// The logger will write logs to [filename]. The logger will create
// the file if it does not exist.
MICRO_LOG_DEF micro_log_error micro_log_set_file(char* filename);

#ifdef MICRO_LOG_SOCKETS

// Set output internet socket of the global logger
//
// Note: You need to have defined MICRO_LOG_SOCKETS before including
// this header in torder to use this function
MICRO_LOG_DEF micro_log_error
micro_log_set_socket_inet(char* addr,
                          int port,
                          MicroLogProto protocol);

#if defined(__unix__) || defined(__unix)

// Set output unix socket of the global logger
//
// Note: You need to have defined MICRO_LOG_SOCKETS before including
// this header, and be in an unix system to use this function.
MICRO_LOG_DEF micro_log_error micro_log_set_socket_unix(char* path);

#endif // __unix__

#endif // MICRO_LOG_SOCKETS

//
// Local logger functions
//
// These are equivalent to the global ones but they take an additional
// MicroLog first argument, which is the logger what will be modified.

MICRO_LOG_DEF micro_log_error micro_log_init2(MicroLog *micro_log);

// micro_log_from_file2 expects [micro_log] to be already initialzied
MICRO_LOG_DEF micro_log_error
micro_log_from_file2(MicroLog *micro_log, char *filename);
  
MICRO_LOG_DEF micro_log_error micro_log_close2(MicroLog *micro_log);
  
MICRO_LOG_DEF micro_log_error micro_log_flush2(MicroLog *micro_log);
  
MICRO_LOG_DEF micro_log_error
micro_log_set_flags2(MicroLog *micro_log,
                     long unsigned int flags_bitfield);
  
MICRO_LOG_DEF micro_log_error
micro_log_set_level2(MicroLog *micro_log,
                     MicroLogLevel level);
  
MICRO_LOG_DEF micro_log_error
micro_log_set_out2(MicroLog *micro_log, int out_flags);
  
MICRO_LOG_DEF micro_log_error
micro_log_set_file2(MicroLog *micro_log,
                    char* filename);
  
#ifdef MICRO_LOG_SOCKETS

MICRO_LOG_DEF micro_log_error
micro_log_set_socket_inet2(MicroLog *micro_log,
                           char* addr,
                           int port,
                           MicroLogProto protocol);

#if defined(__unix__) || defined(__unix)

MICRO_LOG_DEF micro_log_error
micro_log_set_socket_unix2(MicroLog *micro_log,
                           char* path);

#endif // __unix__

#endif // MICRO_LOG_SOCKETS

// Misc

// Inline function that just returns MICRO_LOG_OK
// This is needed when log level is disabled at compile time, and
// it needs a definition in every translation unit to be inlined
static inline int micro_log_disabled(void)
{
  return MICRO_LOG_OK;
}
  
// Get a string of a certain log level, with an optional color
MICRO_LOG_DEF const char*
micro_log_level_string(MicroLogLevel level, bool color);

// The central log function
//
// All other functions call this one for printing. It handles all the
// flags and calls `_micro_log_print_outputs`.
MICRO_LOG_DEF micro_log_error
_micro_log_write_impl(MicroLog *micro_log,
                      MicroLogLevel level,
                      const char* file,
                      int line,
                      const char *fmt, ...);

// Wraps `_micro_log_print_outputs_args`
MICRO_LOG_DEF micro_log_error
_micro_log_print_outputs(MicroLog *micro_log,
                         const char* fmt, ...);

// Print formatted string in multiple outputs.
MICRO_LOG_DEF micro_log_error
_micro_log_print_outputs_args(MicroLog *micro_log,
                              const char* fmt,
                              _micro_log_va_list args);
//
// Implementation
//

#ifdef MICRO_LOG_IMPLEMENTATION

#ifdef MICRO_LOG_DATE_TIME
  #include <time.h>
#endif

#ifdef MICRO_LOG_PID
  #include <pthread.h>
#endif

#ifndef MICRO_STATIC_ASSERT
#define MICRO_STATIC_ASSERT(cond, msg)                  \
  typedef char static_assertion_##msg##__LINE__[(cond) ? 1 : -1]
#endif

#define _micro_log_va_start(v, l)  __builtin_va_start(v, l)
#define _micro_log_va_end(v)      __builtin_va_end(v)
#define _micro_log_va_arg(v, l)    __builtin_va_arg(v, l)
#define _micro_log_va_copy(d, s)   __builtin_va_copy(d, s)

#ifdef MICRO_LOG_SOCKETS
  #ifdef _WIN32
    // Windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define close_socket(s) closesocket(s)
    #pragma comment(lib, "ws2_32.lib")  // link against Winsock library
    #define dprintf(...) ; // TODO
    #define vdprintf(...) ; // TODO
    #error "TODO Support for windows internet sockets"
  #else
    // POSIX (Linux, macOS, BSD, etc.)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <sys/un.h>
    #define close_socket(s) close(s)
  #endif // _WIN32
#endif // MICRO_LOG_SOCKETS

  
#ifdef MICRO_LOG_MULTITHREADED

#define __MICRO_LOG_LOCK(__micro_log_ptr)                   \
  do {                                                      \
    if (pthread_mutex_lock(&__micro_log_ptr->write_mutex) != 0) \
    {                                                       \
      error = MICRO_LOG_ERROR_MUTEX_LOCK;                   \
      goto done;                                            \
    }                                                       \
  } while (0)
#define __MICRO_LOG_UNLOCK(__micro_log_ptr)                   \
  do {                                                        \
    if (pthread_mutex_unlock(&__micro_log_ptr->write_mutex) != 0) \
    {                                                         \
      error = MICRO_LOG_ERROR_MUTEX_UNLOCK;                   \
      goto done;                                              \
    }                                                         \
  } while(0)

#else
  
#define __MICRO_LOG_LOCK(__micro_log_ptr) ;
#define __MICRO_LOG_UNLOCK(__micro_log_ptr) ;

#endif // MICRO_LOG_MULTITHREADED

MicroLog micro_log_global;

MICRO_LOG_DEF micro_log_error micro_log_init(void)
{
  return micro_log_init2(&micro_log_global);
}

MICRO_LOG_DEF micro_log_error micro_log_from_file(char *filename)
{
  return micro_log_from_file2(&micro_log_global, filename);
}

MICRO_LOG_DEF micro_log_error micro_log_close(void)
{
  return micro_log_close2(&micro_log_global);
}

MICRO_LOG_DEF micro_log_error micro_log_flush(void)
{
  return micro_log_flush2(&micro_log_global);
}

MICRO_LOG_DEF micro_log_error
micro_log_set_flags(long unsigned int flags_bitfield)
{
  return micro_log_set_flags2(&micro_log_global, flags_bitfield);
}

MICRO_LOG_DEF micro_log_error micro_log_set_level(MicroLogLevel level)
{
  return micro_log_set_level2(&micro_log_global, level);
}

MICRO_LOG_DEF micro_log_error micro_log_set_out(int out_flags)
{
  return micro_log_set_out2(&micro_log_global, out_flags);
}

MICRO_LOG_DEF micro_log_error micro_log_set_file(char* filename)
{
  return micro_log_set_file2(&micro_log_global, filename);
}

#ifdef MICRO_LOG_SOCKETS
MICRO_LOG_DEF micro_log_error
micro_log_set_socket_inet(char* addr,
                          int port,
                          MicroLogProto protocol)
{
  return micro_log_set_socket_inet2(&micro_log_global, addr, port, protocol);
}

#if defined(__unix__) || defined(__unix)

MICRO_LOG_DEF micro_log_error micro_log_set_socket_unix(char* path)
{
  return micro_log_set_socket_unix2(&micro_log_global, path);
}

#endif // __unix__

#endif // MICRO_LOG_SOCKETS

MICRO_LOG_DEF micro_log_error micro_log_init2(MicroLog *micro_log)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  *micro_log = (MicroLog){
    .log_level = MICRO_LOG_LEVEL_TRACE,
    .flags_bitfield = 0,
    .out_bitfield = MICRO_LOG_OUT_STDOUT,
    .file = NULL,
    #ifdef MICRO_LOG_SOCKETS
    .inet_sock_fd = 0,
    #if defined(__unix__) || defined(__unix)
    .unix_sock_fd = 0,
    #endif // __unix__
    #endif // MICRO_LOG_SOCKETS
  };

  #ifdef MICRO_LOG_MULTITHREADED
  pthread_mutex_init(&micro_log->write_mutex, NULL);
  #endif

  micro_log_info2(micro_log, "Logger initialized");
  return MICRO_LOG_OK;
}

MICRO_LOG_DEF int _micro_log_get_spaces(char* str, int max)
{
  int spaces = 0;
  while((*(str + spaces) == ' ' || *(str + spaces) == '\t') && spaces < max)
    spaces++;
  return spaces;
}

MICRO_LOG_DEF int _micro_log_get_word_len(char* str, int max)
{
  int letters = 0;
  while(*(str+letters) != ' '
        && *(str+letters) != '\n'
        && *(str+letters) != '\t'
        && letters < max)
    letters++;
  return letters;
}

typedef struct {
    void* handle;      // The file handle from micro_platform.open
    char  buffer[512]; // Internal buffer to minimize syscalls
    int   pos;         // Current position in buffer
    int   size;        // Actual bytes currently in buffer
    bool  eof;
} MicroStream;

// A minimal replacement for fgetc/getline logic
static int _micro_log_getc(MicroStream* stream)
{
  if (stream->pos >= stream->size)
  {
    if (stream->eof) return -1;
        
    // Use your platform abstraction
    int bytes = MICRO_LOG_FREAD(stream->buffer,
                                1,
                                sizeof(stream->buffer),
                                stream->handle);
    if (bytes <= 0)
    {
      stream->eof = true;
      return -1;
    }
    stream->size = bytes;
    stream->pos = 0;
  }
  return (unsigned char)stream->buffer[stream->pos++];
}

static size_t _micro_log_getline(char** lineptr, size_t* n, MicroStream* stream)
{
  size_t capacity = (*n > 0) ? *n : 128;
  if (*lineptr == NULL) *lineptr = MICRO_LOG_REALLOC(NULL, capacity);
  
  size_t count = 0;
  int c;

  while ((c = _micro_log_getc(stream)) != -1)
  {
    // Ensure space for char + null terminator
    if (count + 1 >= capacity)
    {
      capacity *= 2;
      char* new_buf = MICRO_LOG_REALLOC(*lineptr, capacity);
      if (!new_buf) return -1;
      *lineptr = new_buf;
      *n = capacity;
    }
    
    (*lineptr)[count++] = (char)c;
    if (c == '\n') break;
  }

  if (count == 0) return -1;
  (*lineptr)[count] = '\0';
  return count;
}
  
static inline int _micro_log_strncmp(const char *s1, const char *s2, size_t n)
{
  while (*s1 == *s2 && n > 0) {
    s1++;
    s2++;
    n--;
  }
  if (*s2 == '\0')
    return 0;
  return 1;
}

MICRO_LOG_DEF micro_log_error
micro_log_from_file2(MicroLog *micro_log, char *filename)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;
  if (filename == NULL)
    return MICRO_LOG_ERROR_FROM_FILE_NULL;

  micro_log_error error = MICRO_LOG_OK;
  
  void* file = MICRO_LOG_FOPEN(filename, MICRO_FILE_MODE_READ);
  if (file == NULL)
  {
    MICRO_LOG_OUT("Error opening file");
    error = MICRO_LOG_ERROR_OPEN_FILE;
    goto done;
  }

  // Example file:
  //
  // level: debug
  // flags: level date time tid pid
  // file: output.txt
  // inet: 127.0.0.1 5000 TCP
  // unix: /tmp/a-socket
  // # A comment
  //
  char *line = NULL;
  size_t len = 0;
  int spaces;
  while(_micro_log_getline(&line, &len, file) > 0)
  {
    if (_micro_log_strncmp(line, "#", 1) == 0)
    {
      goto next;
    }
    if (_micro_log_strncmp(line, "\n", 1) == 0)
    {
      goto next;
    }
    if (_micro_log_strncmp(line, "level:", 6) == 0)
    {
      spaces = _micro_log_get_spaces(line + 6, len - 6);
      
      if (_micro_log_strncmp(line + 6 + spaces, "trace", 5) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_TRACE);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "debug", 5) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_DEBUG);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "info", 4) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_INFO);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "warn", 4) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_WARN);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "error", 5) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_ERROR);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "fatal", 5) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_FATAL);
      }
      else if (_micro_log_strncmp(line + 6 + spaces, "disabled", 8) == 0)
      {
        micro_log_set_level2(micro_log, MICRO_LOG_LEVEL_DISABLED);
      } else {
        error = MICRO_LOG_ERROR_UNKNOWN_LEVEL;
        MICRO_LOG_FREE(line);
        goto done;
      }
    }
    else if (_micro_log_strncmp(line, "flags:", 6) == 0)
    {
      size_t pos = 6;
      long unsigned int flags = 0;
      pos += _micro_log_get_spaces(line + 6, len - 6);
      while (pos <= len)
      {
        if (_micro_log_strncmp(line + pos, "level", 5) == 0)
        {
          flags |= MICRO_LOG_FLAG_LEVEL;
          pos += 5;
        }
        else if (_micro_log_strncmp(line + pos, "date", 4) == 0)
        {
          flags |= MICRO_LOG_FLAG_DATE;
          pos += 4;
        }
        else if (_micro_log_strncmp(line + pos, "time", 4) == 0)
        {
          flags |= MICRO_LOG_FLAG_TIME;
          pos += 4;
        }
        else if (_micro_log_strncmp(line + pos, "pid", 3) == 0)
        {
          flags |= MICRO_LOG_FLAG_PID;
          pos += 3;
        }
        else if (_micro_log_strncmp(line + pos, "tid", 3) == 0)
        {
          flags |= MICRO_LOG_FLAG_TID;
          pos += 3;
        }
        else if (_micro_log_strncmp(line + pos, "json", 4) == 0)
        {
          flags |= MICRO_LOG_FLAG_JSON;
          pos += 4;
        }
        else if (_micro_log_strncmp(line + pos, "color", 5) == 0)
        {
          flags |= MICRO_LOG_FLAG_COLOR;
          pos += 5;
        }
        else if (_micro_log_strncmp(line + pos, "file", 4) == 0)
        {
          flags |= MICRO_LOG_FLAG_FILE;
          pos += 4;
        }
        else if (_micro_log_strncmp(line + pos, "line", 4) == 0)
        {
          flags |= MICRO_LOG_FLAG_LINE;
          pos += 4;
        }
        else {
          error = MICRO_LOG_ERROR_UNKNOWN_FLAG;
          MICRO_LOG_FREE(line);
          goto done;
        }

        if (*(line + pos) == '\n') break;
        pos += _micro_log_get_spaces(line + pos, len - pos);
      }
      error = micro_log_set_flags2(micro_log, flags);
      if (error != MICRO_LOG_OK) { MICRO_LOG_FREE(line); goto done; }
    }
    else if (_micro_log_strncmp(line, "file:", 5) == 0)
    {
      size_t pos = 5;
      pos += _micro_log_get_spaces(line + pos, len - 6);
      int file_str_len = _micro_log_get_word_len(line + pos, len - pos);
      if (pos + file_str_len < len)
      {
        line[pos + file_str_len] = '\0';
      }
      
      error = micro_log_set_file2(micro_log, line + pos);
      if (error != MICRO_LOG_OK) { MICRO_LOG_FREE(line); goto done; }
    }
    #ifdef MICRO_LOG_SOCKETS
    else if (_micro_log_strncmp(line, "inet:", 5) == 0)
    {
      char* addr;
      int port;
      MicroLogProto proto;
      
      int pos = 6;
      pos += _micro_log_get_spaces(line + 6, len - 6);
      int addr_len = _micro_log_get_word_len(line + pos, len - pos);
      if (addr_len == 0)
      {
        error = MICRO_LOG_ERROR_INVALID_INET_ADDR;
        MICRO_LOG_FREE(line);
        goto done;
      }

      addr = MICRO_LOG_MALLOC(addr_len + 1);
      strncpy(addr, line + pos, addr_len);
      addr[addr_len] = '\0';
      
      pos += addr_len;

      pos += _micro_log_get_spaces(line + pos, len - pos);
      int port_len = _micro_log_get_word_len(line + pos, len - pos);
      if (port_len == 0)
      {
        error = MICRO_LOG_ERROR_INVALID_PORT;
        MICRO_LOG_FREE(line); MICRO_LOG_FREE(addr);
        goto done;
      }

      char *port_str = MICRO_LOG_MALLOC(port_len + 1);
      strncpy(port_str, line + pos, port_len);
      port_str[port_len] = '\0';

      port = atoi(port_str);
      if (port_len == 0)
      {
        error = MICRO_LOG_ERROR_INVALID_PORT;
        MICRO_LOG_FREE(port_str);
        MICRO_lOG_FREE(line);
        MICRO_LOG_FREE(addr);
        goto done;
      }
      
      MICRO_LOG_FREE(port_str);
      pos += port_len;

      pos += _micro_log_get_spaces(line + pos, len - pos);
      int proto_len = _micro_log_get_word_len(line + pos, len - pos);
      if (proto_len == 0)
      {
        error = MICRO_LOG_ERROR_INVALID_PROTOCOL;
        MICRO_LOG_FREE(line); MICRO_LOG_FREE(addr);
        goto done;
      }

      if (_micro_log_strncmp(line + pos, "tcp", 3) == 0)
      {
        proto = MICRO_LOG_PROTO_TCP;
      }
      else if (_micro_log_strncmp(line + pos, "udp", 3) == 0)
      {
        proto = MICRO_LOG_PROTO_UDP;
      }
      else {
        error = MICRO_LOG_ERROR_INVALID_PROTOCOL;
        MICRO_LOG_FREE(line); MICRO_LOG_FREE(addr);
        goto done;
      }
      
      error = micro_log_set_socket_inet(addr, port, proto);
      if (error != MICRO_LOG_OK)
      {
        MICRO_LOG_FREE(line);
        MICRO_LOG_FREE(addr);
        goto done;
      }
      
      MICRO_LOG_FREE(addr);
    }
    #if defined(__unix__) || defined(__unix)
    else if (_micro_log_strncmp(line, "unix:", 5) == 0)
    {
      spaces = _micro_log_get_spaces(line + 5, len - 6);
      error = micro_log_set_socket_unix2(micro_log, line + 5 + spaces);
      if (error != MICRO_LOG_OK) { MICRO_LOG_FREE(line); goto done; }
    }
    #endif // __unix__
    #endif // MICRO_LOG_SOCKETS
    else {
      MICRO_LOG_FREE(line);
      error = MICRO_LOG_ERROR_INVALID_FILE_SETTING;
      goto done;
    }

    next:
    MICRO_LOG_FREE(line);
    line = NULL;
  }
  
  if (MICRO_LOG_FCLOSE(file) != 0)
  {
    error = MICRO_LOG_ERROR_CLOSE_FILE;
    goto done;
  }
  
 done:

  if (error == MICRO_LOG_OK)
    micro_log_trace("Initialized logger from file \"%s\"", filename);
  return error;
}

MICRO_LOG_DEF micro_log_error micro_log_close2(MicroLog *micro_log)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  micro_log_error error = MICRO_LOG_OK;

  micro_log_info2(micro_log, "Closing logger");
  
  __MICRO_LOG_LOCK(micro_log);
  
  if (micro_log->file != NULL)
  {
    if (MICRO_LOG_FCLOSE(micro_log->file) != 0)
    {
      MICRO_LOG_OUT("Error closing file");
      error = MICRO_LOG_ERROR_CLOSE_FILE;
      goto done;
    }
  }

  #ifdef MICRO_LOG_SOCKETS
  if (micro_log->inet_sock_fd > 0)
  {
    if (close_socket(micro_log->inet_sock_fd) < 0)
    {
      perror("Error closeing socket");
      error = MICRO_LOG_ERROR_CLOSE_INET_SOCK;
      goto done;
    }
  }
  #if defined(__unix__) || defined(__unix)
  if (micro_log->unix_sock_fd > 0)
  {
    if (close(micro_log->unix_sock_fd) < 0)
    {
      error = MICRO_LOG_ERROR_CLOSE_UNIX_SOCK;
      goto done;
    }
  }
  #endif // __unix__
  #endif // MICRO_LOG_SOCKETS

 done:
  __MICRO_LOG_UNLOCK(micro_log);
  
  #ifdef MICRO_LOG_MULTITHREADED
  pthread_mutex_destroy(&micro_log->write_mutex);
  #endif
  
  return error;
}

MICRO_STATIC_ASSERT(_MICRO_LOG_OUT_MAX == (1 << 4),
                    maybe_should_also_update_micro_log_flush2);
MICRO_LOG_DEF micro_log_error micro_log_flush2(MicroLog *micro_log)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  micro_log_error error = MICRO_LOG_OK;

  if (micro_log->out_bitfield & MICRO_LOG_OUT_STDOUT)
  {
    if (MICRO_LOG_FLUSH(stdout) != 0)
    {
      MICRO_LOG_OUT("Error flushing stdout");
      error = MICRO_LOG_ERROR_FLUSH_STDOUT;
      goto done;
    }
  }
  if (micro_log->out_bitfield & MICRO_LOG_OUT_FILE)
  {
    if (MICRO_LOG_FLUSH(micro_log->file) != 0)
    {
      MICRO_LOG_OUT("Error flushing file");
      error = MICRO_LOG_ERROR_FLUSH_FILE;
      goto done;
    }
  }

 done:
  return error;
}
  
MICRO_LOG_DEF micro_log_error
micro_log_set_flags2(MicroLog *micro_log,
                     long unsigned int flags_bitfield)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;
  
  micro_log_error error = MICRO_LOG_OK;
  
  __MICRO_LOG_LOCK(micro_log);
  
  micro_log->flags_bitfield = flags_bitfield;

  goto done;
 done:
  __MICRO_LOG_UNLOCK(micro_log);
  return error;
}

MICRO_LOG_DEF micro_log_error
micro_log_set_level2(MicroLog *micro_log,
                     MicroLogLevel level)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  if (level > MICRO_LOG_LEVEL_MAX)
    return MICRO_LOG_ERROR_UNKNOWN_LEVEL;
  
  micro_log_error error = MICRO_LOG_OK;

  __MICRO_LOG_LOCK(micro_log);
  
  micro_log->log_level = level;

  goto done;
 done:
  __MICRO_LOG_UNLOCK(micro_log);

  if (error == MICRO_LOG_OK)
    micro_log_trace2(micro_log, "Set log level to %s",
                     micro_log_level_string(level, false));
  return error;
}

MICRO_LOG_DEF micro_log_error
micro_log_set_out2(MicroLog *micro_log, int out_flags)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  micro_log_error error = MICRO_LOG_OK;

  __MICRO_LOG_LOCK(micro_log);
  
  micro_log->out_bitfield = out_flags;

  goto done;
 done:
  __MICRO_LOG_UNLOCK(micro_log);
  return error;
}

MICRO_LOG_DEF micro_log_error
micro_log_set_file2(MicroLog *micro_log,
                    char* filename)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;

  micro_log_error error = MICRO_LOG_OK;
  
  __MICRO_LOG_LOCK(micro_log);
  
  if (micro_log->file != NULL)
  {
    if (MICRO_LOG_FCLOSE(micro_log->file) != 0)
    {
      MICRO_LOG_OUT("Error closing file");
      error = MICRO_LOG_ERROR_CLOSE_FILE;
      goto done;
    }
  }
  
  void *file = MICRO_LOG_FOPEN(filename, MICRO_FILE_MODE_WRITE);
  if (file == NULL)
  {
    MICRO_LOG_OUT("Error opening file");
    error = MICRO_LOG_ERROR_OPEN_FILE;
    goto done;
  }

  micro_log->out_bitfield |= MICRO_LOG_OUT_FILE;
  micro_log->file = file;

 done:
  __MICRO_LOG_UNLOCK(micro_log);

  if (error == MICRO_LOG_OK)
    micro_log_trace("Set output file to \"%s\"", filename);
  return error;
}

#ifdef MICRO_LOG_SOCKETS

MICRO_STATIC_ASSERT(_MICRO_LOG_PROTO_MAX == 2,
                    should_also_update_micro_log_set_socket_inet2);
MICRO_LOG_DEF micro_log_error
micro_log_set_socket_inet2(MicroLog *micro_log,
                           char* addr,
                           int port,
                           MicroLogProto protocol)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;
  if (addr == NULL)
    return MICRO_LOG_ERROR_INET_ADDR_NULL;

  micro_log_error error = MICRO_LOG_OK;

  __MICRO_LOG_LOCK(micro_log);

  if (micro_log->inet_sock_fd > 0)
  {
    if (close_socket(micro_log->inet_sock_fd) < 0)
    {
      MICRO_LOG_OUT("Error closing inet socket");
      error = MICRO_LOG_ERROR_CLOSE_INET_SOCK;
      goto done;
    }
  }

  switch (protocol)
  {
  case MICRO_LOG_PROTO_TCP:
    micro_log->inet_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    break;
  case MICRO_LOG_PROTO_UDP:
    micro_log->inet_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    break;
  default:
    fprintf(stderr, "Inet Protocol unrecognized");
    goto done;
  }
  if (micro_log->inet_sock_fd < 0)
  {
    MICRO_LOG_OUT("Error creating socket");
    error = MICRO_LOG_ERROR_OPEN_INET_SOCK;
    goto done;
  }

  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(port);

  if(inet_pton(AF_INET, addr, &sockaddr_in.sin_addr) <= 0)
  {
    MICRO_LOG_OUT("Error setting inet socket addr");
    error = MICRO_LOG_ERROR_INET_ADDR;
    goto done;
  } 
  
  if (connect(micro_log->inet_sock_fd, (struct sockaddr *) &sockaddr_in,
              sizeof(sockaddr_in)) < 0)
  {
    MICRO_LOG_OUT("Error connecting to inet socket");
    error = MICRO_LOG_ERROR_INET_CONNECT;
    goto done;
  }

  micro_log->out_bitfield |= MICRO_LOG_OUT_SOCK_INET;
  
 done:
  __MICRO_LOG_UNLOCK(micro_log);

  if (error == MICRO_LOG_OK)
    micro_log_trace2(micro_log,
                     "Set output to inet socket at address \"%s\" port %d",
                     addr, port);
  return error;
}

#if defined(__unix__) || defined(__unix)

MICRO_LOG_DEF micro_log_error
micro_log_set_socket_unix2(MicroLog* micro_log, char* path)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;
  if (path == NULL)
    return MICRO_LOG_ERROR_SOCK_PATH_NULL;

  micro_log_error error = MICRO_LOG_OK;

  __MICRO_LOG_LOCK(micro_log);

  if (micro_log->unix_sock_fd > 0)
  {
    if (close(micro_log->unix_sock_fd) < 0)
    {
      MICRO_LOG_OUT("Error closing unix socket");
      error = MICRO_LOG_ERROR_CLOSE_UNIX_SOCK;
      goto done;
    }
  }

  micro_log->unix_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (micro_log->unix_sock_fd < 0)
  {
    MICRO_LOG_OUT("Error creating unix socket");
    error = MICRO_LOG_ERROR_OPEN_UNIX_SOCK;
    goto done;
  }

  struct sockaddr_un sockaddr_un;
  memset(&sockaddr_un, 0, sizeof(sockaddr_un));
  sockaddr_un.sun_family = AF_UNIX;
  strncpy(sockaddr_un.sun_path, path, sizeof(sockaddr_un.sun_path) - 1);
  
  if (connect(micro_log->unix_sock_fd, (struct sockaddr *) &sockaddr_un,
              sizeof(sockaddr_un)) < 0)
  {
    MICRO_LOG_OUT("Error connecting to unix socket");
    error = MICRO_LOG_ERROR_UNIX_CONNECT;
    goto done;
  }

  micro_log->out_bitfield |= MICRO_LOG_OUT_SOCK_UNIX;

 done:
  __MICRO_LOG_UNLOCK(micro_log);

  if (error == MICRO_LOG_OK)
    micro_log_trace2(micro_log,
                     "Set output to unix socket \"%s\"", path);
  return error;
}
#endif // __unix__

#endif // MICRO_LOG_SOCKETS

MICRO_STATIC_ASSERT(MICRO_LOG_LEVEL_MAX == 7,
                    should_also_update_micro_log_level_string);
MICRO_LOG_DEF const char*
micro_log_level_string(MicroLogLevel level, bool color)
{
  switch(level)
  {
  case MICRO_LOG_LEVEL_TRACE:
    return color ? MICRO_LOG_MAG("TRACE") : "TRACE";
  case MICRO_LOG_LEVEL_DEBUG:
    return color ? MICRO_LOG_GRN("DEBUG") : "DEBUG";
  case MICRO_LOG_LEVEL_INFO:
    return color ? MICRO_LOG_CYN("INFO ") : "INFO ";
  case MICRO_LOG_LEVEL_WARN:
    return color ? MICRO_LOG_YEL("WARN ") : "WARN ";
  case MICRO_LOG_LEVEL_ERROR:
    return color ? MICRO_LOG_RED("ERROR") : "ERROR";
  case MICRO_LOG_LEVEL_FATAL:
    return color ? MICRO_LOG_BOLD(MICRO_LOG_RED("FATAL")) : "FATAL";
  case MICRO_LOG_LEVEL_DISABLED:
    return "DISABLED";
  default:
    break;
  }
  return "UNKNOWN";
}

MICRO_LOG_DEF micro_log_error
_micro_log_write_impl(MicroLog *micro_log,
                      MicroLogLevel level,
                      const char* file,
                      int line,
                      const char *fmt, ...)
{
  if (micro_log == NULL)
    return MICRO_LOG_ERROR_LOGGER_NULL;
  if (level < micro_log->log_level
      || micro_log->log_level == MICRO_LOG_LEVEL_DISABLED)
    return MICRO_LOG_OK;

  micro_log_error error = MICRO_LOG_OK;
  
  __MICRO_LOG_LOCK(micro_log);

#define COLOR(x) color ? MICRO_LOG_LGRAY(x) : x
#define COLOR2(x) color ? MICRO_LOG_BOLD(x) : x
#define CHECK_ERROR() if (error != MICRO_LOG_OK) { goto done; }

  // Handle flags
  bool json = false;
  bool color = false;
  
  if (micro_log->flags_bitfield == MICRO_LOG_FLAG_NONE)
  {
    // Skip other flags
    goto do_print;
  }
  
  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_COLOR)
  {
    color = true;
  }
  
  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_JSON)
  {
    json = true;
    color = false;  // json disables color
    error = _micro_log_print_outputs(micro_log, "{ ");
    CHECK_ERROR();
  }

  #ifdef MICRO_LOG_DATE_TIME
  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_DATE)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"date\": \"");
      CHECK_ERROR();
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    error = _micro_log_print_outputs(micro_log, COLOR("%d-%d-%d"),
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }

  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_TIME)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"time\": \"");
      CHECK_ERROR();
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    error = _micro_log_print_outputs(micro_log, COLOR("%d:%d:%d"),
                                     tm.tm_hour, tm.tm_min, tm.tm_sec);
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }
  #endif

  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_LEVEL)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"log_level\": \"");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, "%s",
                                     micro_log_level_string(level, color));
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }

  #ifdef MICRO_LOG_PID
  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_PID)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"pid\": \"");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, COLOR("%d"), getpid());
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }
    
    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }

  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_TID)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"tid\": \"");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, COLOR("%ld"), pthread_self());
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }
    
    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }
  #endif

  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_FILE)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"file\": \"");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, COLOR("%s"), file);
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }
    
    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }

  if (micro_log->flags_bitfield & MICRO_LOG_FLAG_LINE)
  {
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\"line\": \"");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, COLOR("%d"), line);
    CHECK_ERROR();
    
    if (json)
    {
      error = _micro_log_print_outputs(micro_log, "\",");
      CHECK_ERROR();
    }

    error = _micro_log_print_outputs(micro_log, " ");
    CHECK_ERROR();
  }

  if (json)
  {
    error = _micro_log_print_outputs(micro_log, "\"log\": \"");
    CHECK_ERROR();
  }
  else if (micro_log->flags_bitfield > 1)
  {
    error = _micro_log_print_outputs(micro_log, COLOR2("| "));
    CHECK_ERROR();
  }
  
 do_print:;
  _micro_log_va_list args;
  _micro_log_va_start(args, fmt);
  error = _micro_log_print_outputs_args(micro_log, fmt, args);
  if (error != MICRO_LOG_OK) { _micro_log_va_end(args); goto done; }
  _micro_log_va_end(args);

  if (json)
  {
    error = _micro_log_print_outputs(micro_log, "\" }");
    CHECK_ERROR();
  }
  
  error = _micro_log_print_outputs(micro_log, "\n");
  CHECK_ERROR();
  
 done:
  __MICRO_LOG_UNLOCK(micro_log);
  return error;

#undef COLOR
#undef COLOR2
}

static char* _micro_log_strcat_and_advance(char* dest, const char* src)
{
  while (*src)
  {
    *dest++ = *src++;
  }
  return dest;
}

static char* _micro_log_itoa(long val, char* buf, int base)
{
  char* p = buf;
  char* p1, *p2;
  unsigned long uval = (val < 0 && base == 10) ? -val : val;

  do {
    *p++ = "0123456789abcdef"[uval % base];
  } while (uval /= base);

  if (val < 0 && base == 10) *p++ = '-';
  *p = '\0';

  p1 = buf; p2 = p - 1;
  while (p1 < p2)
  {
    char tmp = *p1; *p1++ = *p2; *p2-- = tmp;
  }
  return buf;
}

static void _micro_log_vformat(char* out, const char* fmt,
                               _micro_log_va_list args)
{
  while (*fmt)
  {
    if (*fmt == '%')
    {
      fmt++;
      switch (*fmt)
      {
      case 'd':
      case 'i': {
        char buf[32];
        _micro_log_itoa(_micro_log_va_arg(args, int), buf, 10);
        out = _micro_log_strcat_and_advance(out, buf);
        break;
      }
      case 'x': {
        char buf[32];
        _micro_log_itoa(_micro_log_va_arg(args, unsigned int), buf, 16);
        out = _micro_log_strcat_and_advance(out, buf);
        break;
      }
      case 's': {
        char* s = _micro_log_va_arg(args, char*);
        if (!s) s = "(null)";
        out = _micro_log_strcat_and_advance(out, s);
        break;
      }
      case 'f': {
        double f = _micro_log_va_arg(args, double);
        char buf[64];
        // Handle negative
        if (f < 0) { *out++ = '-'; f = -f; }
        // Integer part
        long i_part = (long)f;
        _micro_log_itoa(i_part, buf, 10);
        out = _micro_log_strcat_and_advance(out, buf);
        *out++ = '.';
        // Fractional part (6 decimal places)
        long f_part = (long)((f - (double)i_part) * 1000000.0 + 0.5);
        _micro_log_itoa(f_part, buf, 10);
        // Add leading zeros to fraction if necessary
        int len = 0; while(buf[len]) len++;
        for(int z = 0; z < (6 - len); z++) *out++ = '0';
        out = _micro_log_strcat_and_advance(out, buf);
        break;
      }
      case '%': {
        *out++ = '%';
        break;
      }
      default: {
        *out++ = '%';
        *out++ = *fmt;
        break;
      }
      }
    }
    else
    {
      *out++ = *fmt;
    }
    fmt++;
  }
  *out = '\0';
}

MICRO_LOG_DEF void micro_log_format(char* out, const char* fmt, ...)
{
  _micro_log_va_list args;
  _micro_log_va_start(args, fmt);

  _micro_log_vformat(out, fmt, args);
  
  _micro_log_va_end(args);
}

MICRO_LOG_DEF micro_log_error
_micro_log_print_outputs_args(MicroLog *micro_log, const char* fmt,
                              _micro_log_va_list args)
{
  micro_log_error error = MICRO_LOG_OK;
  char buff[MICRO_LOG_MAX_STRING_SIZE] = {0};

  _micro_log_vformat(buff, fmt, args);
  
  if (micro_log->out_bitfield & MICRO_LOG_OUT_STDOUT)
  {
    if (MICRO_LOG_OUT(buff) < 0)
    {
      error = MICRO_LOG_ERROR_PRINTF_STDOUT;
      goto done;
    }
  }
  if (micro_log->out_bitfield & MICRO_LOG_OUT_FILE)
  {
    if (MICRO_LOG_OUT(buff) < 0)
    {
      error = MICRO_LOG_ERROR_PRINTF_FILE;
      goto done;
    }
  }
  #ifdef MICRO_LOG_SOCKETS
  if (micro_log->out_bitfield & MICRO_LOG_OUT_SOCK_INET)
  {
    if (write(micro_log->inet_sock_fd, buff, _micro_log_strlen(buff)) < 0)
    {
      error = MICRO_LOG_ERROR_VPRINTF_SOCK_INET;
      goto done;
    }
  }
  #if defined(__unix__) || defined(__unix)
  if (micro_log->out_bitfield & MICRO_LOG_OUT_SOCK_UNIX)
  {
    if (write(micro_log->unix_sock_fd, buff, _micro_log_strlen(buff)) < 0)
    {
      error = MICRO_LOG_ERROR_VPRINTF_SOCK_INET;
      goto done;
    }
  }
  #endif // __unix__
  #endif // MICRO_LOG_SOCKETS

 done:
  return error;  
}

MICRO_LOG_DEF micro_log_error
_micro_log_print_outputs(MicroLog *micro_log, const char* fmt, ...)
{
  micro_log_error ret;
  
  _micro_log_va_list args;
  _micro_log_va_start(args, fmt);
  ret = _micro_log_print_outputs_args(micro_log, fmt, args);
  _micro_log_va_end(args);

  return ret;
}

#undef __MICRO_LOG_LOCK
#undef __MICRO_LOG_UNLOCK

#endif // MICRO_LOG_IMPLEMENTATION

//
// Examples
//

#if 0
#define MICRO_LOG_IMPLEMENTATION
#define MICRO_LOG_MULTITHREADED
#include "micro-log.h"

int main(void)
{
  micro_log_init();
  micro_log_set_flags(MICRO_LOG_FLAG_LEVEL 
                      | MICRO_LOG_FLAG_DATE  
                      | MICRO_LOG_FLAG_TIME);
  micro_log_set_file("out");
  
  micro_log_info("I’d just like to interject for a moment...");
  
  micro_log_close();  
  return 0;
}

#endif // 0

#ifdef __cplusplus
}
#endif

#endif // MICRO_LOG
