#pragma once

#include "vec2.hxx"

struct mouse_state {
  void* method_table;
  void* buttons;
  void* last_source;
  bool is_position_valid;
  char _pad1[ 0x3 ];
  vec2 scroll;
  vec2 position;
  char _pad2[ 0xC ];
};

struct keyboard_state {
  void* method_table;
  void* keys;
};

struct input_state {
  void* method_table;
  mouse_state* mouse;
  keyboard_state* keyboard;
  void* touch;
  void* joystick;
  void* midi;
  void* tablet;
};
