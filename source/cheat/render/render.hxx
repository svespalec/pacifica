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

  inline ImDrawList* get_bg_draw_list() {
    auto* ctx = ImGui::GetCurrentContext();

    if ( !ctx )
      return nullptr;

    return ImGui::GetBackgroundDrawList();
  }

  inline ImDrawList* get_fg_draw_list() {
    return ImGui::GetForegroundDrawList();
  }

  inline void draw_line( vec2 from, vec2 to, color col, float thickness = 1.0f ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddLine( ImVec2( from.x, from.y ), ImVec2( to.x, to.y ), col.to_imu32(), thickness );
  }

  inline void draw_rect( vec2 pos, vec2 size, color col, float rounding = 0.0f, float thickness = 1.0f ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddRect( ImVec2( pos.x, pos.y ), ImVec2( pos.x + size.x, pos.y + size.y ), col.to_imu32(), rounding, 0, thickness );
  }

  inline void draw_circle( vec2 pos, float radius, color col, float thickness = 1.0f ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddCircle( ImVec2( pos.x, pos.y ), radius, col.to_imu32(), 0, thickness );
  }

  inline void fill_rect( vec2 pos, vec2 size, color col, float rounding = 0.0f ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddRectFilled( ImVec2( pos.x, pos.y ), ImVec2( pos.x + size.x, pos.y + size.y ), col.to_imu32(), rounding );
  }

  inline void fill_circle( vec2 pos, float radius, color col ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddCircleFilled( ImVec2( pos.x, pos.y ), radius, col.to_imu32() );
  }

  inline void draw_text( vec2 pos, const char* text, color col ) {
    auto* dl = get_bg_draw_list();

    if ( !dl )
      return;

    dl->AddText( ImVec2( pos.x, pos.y ), col.to_imu32(), text );
  }

  inline vec2 get_screen_size() {
    auto& io = ImGui::GetIO();

    return vec2{ io.DisplaySize.x, io.DisplaySize.y };
  }
} // namespace render
