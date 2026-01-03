#pragma once

#include <cstdint>
#include <imgui.h>
#include <sdk/vec2.hxx>

namespace render {
  struct color {
    uint8_t r, g, b, a;

    color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 ) : r( r ), g( g ), b( b ), a( a ) {}

    ImU32 to_imu32() const {
      return IM_COL32( r, g, b, a );
    }
  };

  ImDrawList* get_bg_draw_list();
  ImDrawList* get_fg_draw_list();

  void draw_line( vec2 from, vec2 to, color col, float thickness = 1.0f );
  void draw_rect( vec2 pos, vec2 size, color col, float rounding = 0.0f, float thickness = 1.0f );
  void draw_circle( vec2 pos, float radius, color col, float thickness = 1.0f );
  void fill_rect( vec2 pos, vec2 size, color col, float rounding = 0.0f );
  void fill_circle( vec2 pos, float radius, color col );
  void draw_text( vec2 pos, const char* text, color col );
  vec2 get_screen_size();
} // namespace render
