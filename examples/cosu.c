// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

//
// cosu!
// =====
//
// osu!mania, but in C.
//

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>

#define RGFW_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/rgfw_platform.h>

// Screen size
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 800
// Vertical speed of the notes
#define SCROLL_SPEED 1
// Number of frames that should be rendered per second
#define FPS 60
// Notes per second to be generated if random mode is set to true
#define NOTE_FREQUENCY 8

#define COSU_KEY_1 MICRO_KEY_D
#define COSU_KEY_2 MICRO_KEY_F
#define COSU_KEY_3 MICRO_KEY_J
#define COSU_KEY_4 MICRO_KEY_K

typedef enum {
  COSU_TYPE_NOTE      = 0,
  COSU_TYPE_SLIDER    = 1,
  COSU_TYPE_NEW_COMBO = 2,
  COSU_TYPE_SPINNER   = 3,
  COSU_TYPE_HOLD_NOTE = 7,
  COSU_TYPE_ERROR     = -1,
} CosuNoteType;

// A single note
typedef struct {
  CosuNoteType type;
  long int x;
  long int y;
  long int time;
} CosuNote;

struct CosuNoteListElem;
typedef struct CosuNoteListElem CosuNoteListElem;

struct CosuNoteListElem {
  CosuNoteListElem *next;
  CosuNoteListElem *prev;
  CosuNote note;
};

typedef struct {
  CosuNoteListElem *head;
  CosuNoteListElem *tail;
} CosuNoteList;

// The cosu! game
typedef struct {
  // Window data
  int    window_width;
  int    window_height;
  // Array of pixels of size
  // screen_width*screen_height*micro_draw_get_channels(MICRO_DRAW_RGBA8)
  MicroDrawCanvas canvas;
  // List of notes during gameplay
  CosuNoteList    notes;
  // See NOTE_FREQUENCY
  double note_frequency;
  // See SCROLL_SPEED
  double scroll_speed;
  // Time since last frame rendered
  double delta_time;
  // Time since last generated note for random mode, or time since the
  // start for map mode
  double note_time;
  // See FPS
  int fps;
  // Keys
  bool key_1_pressed;
  bool key_2_pressed;
  bool key_3_pressed;
  bool key_4_pressed;
} Game;

Game game = {0};

// LCG pseudo random number generator
#define MAGIC1 16843009  // a
#define MAGIC2 826366247 // c
unsigned int lcg_seed = 69696969;
unsigned int lcg(const unsigned int seed)
{
  return (MAGIC1 * seed + MAGIC2);
}

void cosu_note_list_push_back(CosuNoteList *list,
                              CosuNote note)
{
  if (!list) return;
  
  CosuNoteListElem *new_elem =
    (CosuNoteListElem*) MICRO_HEADERS_MALLOC(sizeof(CosuNoteListElem));

  if (!list->tail)
  {
    new_elem->next = NULL;
    new_elem->prev = NULL;
    new_elem->note = note;
    list->tail = new_elem;
    list->head = new_elem;
    return;
  }
  
  new_elem->next = NULL;
  new_elem->prev = list->tail;
  new_elem->note = note;
  list->tail->next = new_elem;
  list->tail = new_elem;
  return;
}

void cosu_note_list_push_front(CosuNoteList *list,
                               CosuNote note)
{
  if (!list) return;
  
  CosuNoteListElem *new_elem =
    (CosuNoteListElem*) MICRO_HEADERS_MALLOC(sizeof(CosuNoteListElem));

  if (!list->head)
  {
    new_elem->next = NULL;
    new_elem->prev = NULL;
    new_elem->note = note;
    list->tail = new_elem;
    list->head = new_elem;
    return;
  }
  
  new_elem->next = list->head;
  new_elem->prev = NULL;
  new_elem->note = note;
  list->head->prev = new_elem;
  list->head = new_elem;
  return;
}

void cosu_note_list_pop_back(CosuNoteList *list)
{
  if (!list) return;
  if (!list->tail) return;
  
  if (list->tail->prev == NULL)
  {
    assert(list->head == list->tail);
    
    MICRO_HEADERS_FREE(list->tail);
    list->head = NULL;
    list->tail = NULL;
    return;
  }

  CosuNoteListElem *tail = list->tail;
  list->tail->prev->next = NULL;
  list->tail = list->tail->prev;
  MICRO_HEADERS_FREE(tail);
  return;
}

void cosu_note_list_pop_front(CosuNoteList *list)
{
  if (!list) return;
  if (!list->head) return;
  
  if (list->head->next == NULL)
  {
    assert(list->head == list->tail);

    MICRO_HEADERS_FREE(list->head);
    list->head = NULL;
    list->tail = NULL;
    return;
  }

  CosuNoteListElem* head = list->head;
  list->head->next->prev = NULL;
  list->head = list->head->next;
  MICRO_HEADERS_FREE(head);
  return;
}

CosuNote* cosu_note_list_head(CosuNoteList *list)
{
  if (!list || !list->head) return NULL;
  return &list->head->note;
}

CosuNote* cosu_note_list_tail(CosuNoteList *list)
{
  if (!list || !list->tail) return NULL;
  return &list->tail->note;
}

int cosu_note_list_len(CosuNoteList *list)
{
  if (!list) return 0;

  CosuNoteListElem *it = list->head;
  int count = 0;
  while(it)
  {
    count++;
    it = it->next;
  }
  
  return count;
}

void cosu_note_list_elem_free(CosuNoteListElem *elem)
{
  if (!elem) return;
  
  CosuNoteListElem *next = elem->next;
  MICRO_HEADERS_FREE(elem);
  cosu_note_list_elem_free(next);
  return;
}

void cosu_note_list_free(CosuNoteList *list)
{
  if (!list) return;

  cosu_note_list_elem_free(list->head);
  return;
}

void cosu_note_print(CosuNote object)
{
  micro_log_trace("x: %ld, y: %ld, time: %ld, type: %d\n",
                  object.x, object.y, object.time, object.type);
  return;
}

void cosu_note_list_elem_print(CosuNoteListElem *elem)
{
  if (!elem) return;
  
  cosu_note_print(elem->note);
  cosu_note_list_elem_print(elem->next);
  return;
}

void cosu_note_list_print(CosuNoteList *list)
{
  if (!list) return;
  cosu_note_list_elem_print(list->head);
  return;
}

bool micro_app_setup(int argc, char** argv)
{
  (void) argc;
  (void) argv;
  
  game.window_width   = WINDOW_WIDTH;
  game.window_height  = WINDOW_HEIGHT;
  game.fps            = FPS;
  game.note_frequency = NOTE_FREQUENCY;
  game.scroll_speed   = SCROLL_SPEED;
  
  micro_platform.init("cosu!", game.window_width, game.window_height);

  micro_log_init();
  micro_log_set_level(MICRO_LOG_LEVEL_TRACE);
  micro_log_set_flags(MICRO_LOG_FLAG_LEVEL
                      | MICRO_LOG_FLAG_DATE
                      | MICRO_LOG_FLAG_TIME);
  micro_draw_canvas_init(&game.canvas,
                         game.window_width,
                         game.window_height,
                         MICRO_DRAW_RGBA8);
  return true;
}

bool micro_app_update(float delta_time)
{
  game.delta_time += delta_time / 1000;
  game.note_time  += delta_time / 1000;

  if (game.delta_time < 1.0 / (double) game.fps)
    return true;

  game.delta_time = 0;
  
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;

  CosuNote* note = cosu_note_list_tail(&game.notes);

  if (note)
  {
    bool hit = false;

    // Key 1
    if (micro_platform.get_key(COSU_KEY_1) && !game.key_1_pressed)
    {
      game.key_1_pressed = true;
      if (note->x == 0 && note->y > 3 * (game.window_height / 4))
        hit = true;
    }
    else
    {
      game.key_1_pressed = false;
    }

    // Key 2
    if (micro_platform.get_key(COSU_KEY_2) && !game.key_2_pressed)
    {
      game.key_2_pressed = true;
      if (note->x == 1 && note->y > 3 * (game.window_height / 4))
        hit = true;
    }
    else
    {
      game.key_2_pressed = false;
    }

    // Key 3
    if (micro_platform.get_key(COSU_KEY_3) && !game.key_3_pressed)
    {
      game.key_3_pressed = true;
      if (note->x == 2 && note->y > 3 * (game.window_height / 4))
        hit = true;
    }
    else
    {
      game.key_3_pressed = false;
    }

    // Key 4
    if (micro_platform.get_key(COSU_KEY_4) && !game.key_4_pressed)
    {
      game.key_4_pressed = true;
      if (note->x == 3 && note->y > 3 * (game.window_height / 4))
        hit = true;
    }
    else
    {
      game.key_4_pressed = false;
    }
  
    if (hit)
      cosu_note_list_pop_back(&game.notes);
  }

  // Remove notes that fall outside of the screen, and update
  // all positions
  CosuNoteListElem *it = game.notes.tail;
  while(it)
  {
    if (it->note.y > game.window_height) // Note is out of screen
    {
      it = it->prev;
      cosu_note_list_pop_back(&game.notes);
      micro_log_trace("missed note");
      continue;
    }

    // Update note position
    it->note.y += 1.0 / (double)game.fps * game.window_height * game.scroll_speed;
    it = it->prev;
  }
  
  // Generate notes randomly

  if (game.note_time > 1 / (double) game.note_frequency)
  {    
    game.note_time = 0;
    
    lcg_seed = lcg(lcg_seed);
    if (lcg_seed % 5 != 4) // module 5 to allow empty space, also
                           // lcg does not work modulo 4
    {
      CosuNote note = (CosuNote) {
        .type = COSU_TYPE_NOTE,
        .x = (lcg_seed % 5),
        .y = 0
      };
      cosu_note_list_push_front(&game.notes, note);
      // micro_log_trace("Created note at position x: %d, y: %d", note.x, note.y);
    }
  }
  
  return true;
}

bool micro_app_draw(void)
{
  unsigned char color_black[4] = {0, 0, 0, 255};
  unsigned char color_red[4] = {255, 0, 0, 255};
  unsigned char color_white[4] = {255, 255, 255, 255};
  micro_draw_clear(&game.canvas, color_black);

  CosuNoteListElem *it = game.notes.tail;
  while(it)
  {
    // Hit line
    micro_draw_line(&game.canvas,
                    (Vec2) {
                      .x = 0,
                      .y = 3 * (game.window_height / 4) + game.window_height / 10,
                    },
                    (Vec2) {
                      .x = game.window_width,
                      .y = 3 * (game.window_height / 4) + game.window_height / 10,
                    },
                    color_red);

    unsigned char *note_color =
      (it->note.y > 3 * (game.window_height / 4) + game.window_height / 10)
      ? color_red : color_white;
    
    // Note
    micro_draw_fill_rect(&game.canvas,
                         (Rect) {
                           .d_x = it->note.x * game.window_width / 4,
                           .d_y = it->note.y,
                           .c_x = it->note.x * game.window_width / 4,
                           .c_y = it->note.y + game.window_height / 10,
                           .b_x = it->note.x * game.window_width / 4 + game.window_width / 4,
                           .b_y = it->note.y + game.window_height / 10,
                           .a_x = it->note.x * game.window_width / 4 + game.window_width / 4,
                           .a_y = it->note.y,
                         },
                         note_color);

    it = it->prev;
  }

  micro_platform.draw_frame(game.canvas.data,
                            game.canvas.width,
                            game.canvas.height);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&game.canvas);
  micro_log_close();
  micro_platform.terminate();
}
