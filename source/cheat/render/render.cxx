#include "render.hxx"

ImDrawList* render::get_bg_draw_list() {
  auto* ctx = ImGui::GetCurrentContext();

  if ( !ctx )
    return nullptr;

  return ImGui::GetBackgroundDrawList();
}

ImDrawList* render::get_fg_draw_list() {
  return ImGui::GetForegroundDrawList();
}

void render::draw_line( vec2 from, vec2 to, color col, float thickness ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddLine( ImVec2( from.x, from.y ), ImVec2( to.x, to.y ), col.to_imu32(), thickness );
}

void render::draw_rect( vec2 pos, vec2 size, color col, float rounding, float thickness ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddRect( ImVec2( pos.x, pos.y ), ImVec2( pos.x + size.x, pos.y + size.y ), col.to_imu32(), rounding, 0, thickness );
}

void render::draw_circle( vec2 pos, float radius, color col, float thickness ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddCircle( ImVec2( pos.x, pos.y ), radius, col.to_imu32(), 0, thickness );
}

void render::fill_rect( vec2 pos, vec2 size, color col, float rounding ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddRectFilled( ImVec2( pos.x, pos.y ), ImVec2( pos.x + size.x, pos.y + size.y ), col.to_imu32(), rounding );
}

void render::fill_circle( vec2 pos, float radius, color col ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddCircleFilled( ImVec2( pos.x, pos.y ), radius, col.to_imu32() );
}

void render::draw_text( vec2 pos, const char* text, color col ) {
  auto* dl = get_bg_draw_list();

  if ( !dl )
    return;

  dl->AddText( ImVec2( pos.x, pos.y ), col.to_imu32(), text );
}

vec2 render::get_screen_size() {
  auto& io = ImGui::GetIO();

  return vec2 { io.DisplaySize.x, io.DisplaySize.y };
}
