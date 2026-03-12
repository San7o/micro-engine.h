// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef WASM_PLATFORM_H
#define WASM_PLATFORM_H

#include "../micro-platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// Implemented in javascript
extern bool js_platform_init(const char* title, int width, int height);
extern bool js_platform_terminate();
extern void js_platform_draw_frame(unsigned char* data, int width, int height);
extern void js_platform_sleep_ms(unsigned int ms);
extern unsigned long int js_platform_get_ticks_ms(void);
extern bool js_platform_is_key_down(const char* w3c_keycode);
extern int  js_platform_print(const char* msg);

//
// Implementation
//

#ifdef WASM_PLATFORM_IMPLEMENTATION

#ifndef MICRO_ENGINE_MEMORY_SIZE
  #define MICRO_ENTINE_MEMORY_SIZE (5 * 1024 * 1024)
#endif
char buff[MICRO_ENGINE_MEMORY_SIZE];

static bool wasm_platform_init(const char* title, int width, int height)
{
  micro_arena_ginit(&buff[0], MICRO_ENGINE_MEMORY_SIZE);
  return js_platform_init(title, width, height);
}

static bool wasm_platform_terminate(void)
{
  return true;
}

static void wasm_pool_events(void)
{
  return;
}

static void wasm_platform_draw_frame(unsigned char* data, int width, int height)
{
  js_platform_draw_frame(data, width, height);
  return;
}
  
static void wasm_platform_sleep_ms(unsigned int ms)
{
  js_platform_sleep_ms(ms);
  return;
}

static unsigned long int wasm_platform_get_ticks_ms(void)
{
  return js_platform_get_ticks_ms();
}

// Converts key to W3C KeyboardEvent code string
static const char* _micro_key_to_js_code(MicroKey key)
{
  switch (key)
  {
  // PRINTABLE KEYS
  case MICRO_KEY_SPACE:         return "Space";
  case MICRO_KEY_APOSTROPHE:    return "Quote";
  case MICRO_KEY_COMMA:         return "Comma";
  case MICRO_KEY_MINUS:         return "Minus";
  case MICRO_KEY_PERIOD:        return "Period";
  case MICRO_KEY_SLASH:         return "Slash";
  case MICRO_KEY_D0:            return "Digit0";
  case MICRO_KEY_D1:            return "Digit1";
  case MICRO_KEY_D2:            return "Digit2";
  case MICRO_KEY_D3:            return "Digit3";
  case MICRO_KEY_D4:            return "Digit4";
  case MICRO_KEY_D5:            return "Digit5";
  case MICRO_KEY_D6:            return "Digit6";
  case MICRO_KEY_D7:            return "Digit7";
  case MICRO_KEY_D8:            return "Digit8";
  case MICRO_KEY_D9:            return "Digit9";
  case MICRO_KEY_SEMICOLON:     return "Semicolon";
  case MICRO_KEY_EQUAL:         return "Equal";
  case MICRO_KEY_A:             return "KeyA";
  case MICRO_KEY_B:             return "KeyB";
  case MICRO_KEY_C:             return "KeyC";
  case MICRO_KEY_D:             return "KeyD";
  case MICRO_KEY_E:             return "KeyE";
  case MICRO_KEY_F:             return "KeyF";
  case MICRO_KEY_G:             return "KeyG";
  case MICRO_KEY_H:             return "KeyH";
  case MICRO_KEY_I:             return "KeyI";
  case MICRO_KEY_J:             return "KeyJ";
  case MICRO_KEY_K:             return "KeyK";
  case MICRO_KEY_L:             return "KeyL";
  case MICRO_KEY_M:             return "KeyM";
  case MICRO_KEY_N:             return "KeyN";
  case MICRO_KEY_O:             return "KeyO";
  case MICRO_KEY_P:             return "KeyP";
  case MICRO_KEY_Q:             return "KeyQ";
  case MICRO_KEY_R:             return "KeyR";
  case MICRO_KEY_S:             return "KeyS";
  case MICRO_KEY_T:             return "KeyT";
  case MICRO_KEY_U:             return "KeyU";
  case MICRO_KEY_V:             return "KeyV";
  case MICRO_KEY_W:             return "KeyW";
  case MICRO_KEY_X:             return "KeyX";
  case MICRO_KEY_Y:             return "KeyY";
  case MICRO_KEY_Z:             return "KeyZ";
  case MICRO_KEY_LEFTBRACKET:   return "BracketLeft";
  case MICRO_KEY_BACKSLASH:     return "Backslash";
  case MICRO_KEY_RIGHTBRACKET:  return "BracketRight";
  case MICRO_KEY_GRAVEACCENT:   return "Backquote";

  // FUNCTION KEYS
  case MICRO_KEY_ESCAPE:        return "Escape";
  case MICRO_KEY_ENTER:         return "Enter";
  case MICRO_KEY_TAB:            return "Tab";
  case MICRO_KEY_BACKSPACE:      return "Backspace";
  case MICRO_KEY_INSERT:         return "Insert";
  case MICRO_KEY_DELETE:         return "Delete";
  case MICRO_KEY_RIGHT:          return "ArrowRight";
  case MICRO_KEY_LEFT:           return "ArrowLeft";
  case MICRO_KEY_DOWN:           return "ArrowDown";
  case MICRO_KEY_UP:             return "ArrowUp";
  case MICRO_KEY_PAGEUP:         return "PageUp";
  case MICRO_KEY_PAGEDOWN:       return "PageDown";
  case MICRO_KEY_HOME:           return "Home";
  case MICRO_KEY_END:            return "End";
  case MICRO_KEY_CAPSLOCK:       return "CapsLock";
  case MICRO_KEY_SCROLLLOCK:     return "ScrollLock";
  case MICRO_KEY_NUMLOCK:        return "NumLock";
  case MICRO_KEY_PRINTSCREEN:    return "PrintScreen";
  case MICRO_KEY_PAUSE:          return "Pause";
  case MICRO_KEY_F1:             return "F1";
  case MICRO_KEY_F2:             return "F2";
  case MICRO_KEY_F3:             return "F3";
  case MICRO_KEY_F4:             return "F4";
  case MICRO_KEY_F5:             return "F5";
  case MICRO_KEY_F6:             return "F6";
  case MICRO_KEY_F7:             return "F7";
  case MICRO_KEY_F8:             return "F8";
  case MICRO_KEY_F9:             return "F9";
  case MICRO_KEY_F10:            return "F10";
  case MICRO_KEY_F11:            return "F11";
  case MICRO_KEY_F12:            return "F12";

  // KEYPAD
  case MICRO_KEY_KP0:            return "Numpad0";
  case MICRO_KEY_KP1:            return "Numpad1";
  case MICRO_KEY_KP2:            return "Numpad2";
  case MICRO_KEY_KP3:            return "Numpad3";
  case MICRO_KEY_KP4:            return "Numpad4";
  case MICRO_KEY_KP5:            return "Numpad5";
  case MICRO_KEY_KP6:            return "Numpad6";
  case MICRO_KEY_KP7:            return "Numpad7";
  case MICRO_KEY_KP8:            return "Numpad8";
  case MICRO_KEY_KP9:            return "Numpad9";
  case MICRO_KEY_KPDECIMAL:      return "NumpadDecimal";
  case MICRO_KEY_KPDIVIDE:       return "NumpadDivide";
  case MICRO_KEY_KPMULTIPLY:     return "NumpadMultiply";
  case MICRO_KEY_KPSUBTRACT:     return "NumpadSubtract";
  case MICRO_KEY_KPADD:          return "NumpadAdd";
  case MICRO_KEY_KPENTER:        return "NumpadEnter";
  case MICRO_KEY_KPEQUAL:        return "NumpadEqual";

  // MODIFIERS
  case MICRO_KEY_LEFTSHIFT:      return "ShiftLeft";
  case MICRO_KEY_LEFTCONTROL:    return "ControlLeft";
  case MICRO_KEY_LEFTALT:        return "AltLeft";
  case MICRO_KEY_LEFTSUPER:      return "MetaLeft";
  case MICRO_KEY_RIGHTSHIFT:     return "ShiftRight";
  case MICRO_KEY_RIGHTCONTROL:   return "ControlRight";
  case MICRO_KEY_RIGHTALT:       return "AltRight";
  case MICRO_KEY_RIGHTSUPER:     return "MetaRight";
  case MICRO_KEY_MENU:           return "ContextMenu";

  default:                       return "";
  }
}

static bool wasm_platform_get_key(MicroKey key)
{
  const char* js_key = _micro_key_to_js_code(key);
  if (js_key[0] == '\0') return false;
  return js_platform_is_key_down(js_key);
}

static int wasm_platform_print(const char* msg)
{
  return js_platform_print(msg);
}

static void* wasm_platform_open(const char* path, MicroFileMode mode)
{
  (void) path;
  (void) mode;
  return NULL;
}

static bool wasm_platform_close(void* handle)
{
  (void) handle;
  return false;
}

static size_t wasm_platform_read(void* handle, void* buffer, size_t size)
{
  (void) handle;
  (void) buffer;
  (void) size;
  return 0;
}

static size_t wasm_platform_write(void* handle, void* buffer, size_t size)
{
  (void) handle;
  (void) buffer;
  (void) size;
  return 0;
}

// Provide a platform
MicroPlatform micro_platform = {
  .init              = wasm_platform_init,
  .terminate         = wasm_platform_terminate,
  .pool_events       = wasm_pool_events,
  .draw_frame        = wasm_platform_draw_frame,
  .sleep_ms          = wasm_platform_sleep_ms,
  .get_ticks_ms      = wasm_platform_get_ticks_ms,
  .get_key           = wasm_platform_get_key,
  .print             = wasm_platform_print,
  .open              = wasm_platform_open,
  .close             = wasm_platform_close,
  .read              = wasm_platform_read,
  .write             = wasm_platform_write,
};

#endif // WASM_PLATFORM_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // WASM_PLATFORM_H
