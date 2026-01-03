#include "aim.hxx"

#include <utils/utils.hxx>
#include <hook/hook.hxx>
#include <sdk/sdk.hxx>
#include <render/render.hxx>

namespace features::aim {

  // state
  static player* g_player = nullptr;
  static vec2 g_offset = {};
  static vec2 g_cursor_pos = {};   // raw cursor position (before offset)
  static vec2 g_adjusted_pos = {}; // adjusted position (after offset)

  // find the next hit object we should aim at
  static hit_object* find_target( double current_time ) {
    if ( !g_player || !g_player->state || !g_player->state->map )
      return nullptr;

    auto* map = g_player->state->map;

    if ( !map->hit_objects )
      return nullptr;

    auto* objects = map->hit_objects;
    int count = objects->size;

    if ( count <= 0 )
      return nullptr;

    // look for the next object that hasn't been hit yet
    // (start_time > current_time - small_window)
    for ( int i = 0; i < count; i++ ) {
      auto* obj = get_list_item< hit_object >( objects, i );

      if ( !obj )
        continue;

      double start = obj->get_start_time();

      // find object that's coming up (within 1 second) or just passed (50ms grace)
      if ( start > current_time - 50.0 && start < current_time + 1000.0 )
        return obj;
    }

    return nullptr;
  }

  static vec2 process( vec2 pos ) {
    // store raw cursor pos for overlay
    g_cursor_pos = pos;

    // always decay offset so never sudden changes
    g_offset = g_offset * config::decay;

    double time = g_player->get_current_time();
    auto* obj = find_target( time );

    if ( !obj ) {
      g_adjusted_pos = pos + g_offset;
      return g_adjusted_pos;
    }

    vec2 target = g_scaler.to_screen( obj->get_position() );
    vec2 to_target = target - ( pos + g_offset );

    float dist = to_target.length();

    float cs = g_player->state->map->difficulty->circle_size;
    float radius = 54.4f - 4.48f * cs;
    float assist_radius = g_scaler.scale_value( radius ) + config::fov;

    // in range, add tiny pull toward target
    if ( dist < assist_radius && dist > 1.0f )
      g_offset = g_offset + to_target * config::pull;

    g_adjusted_pos = pos + g_offset;

    return g_adjusted_pos;
  }

  void draw() {
    if ( !config::display_overlay )
      return;

    if ( !g_player || !g_player->is_playing() )
      return;

    if ( !g_player->state || !g_player->state->map || !g_player->state->map->difficulty )
      return;

    // sync scaler with current window size
    g_scaler.update();

    double time = g_player->get_current_time();
    auto* obj = find_target( time );

    float cs = g_player->state->map->difficulty->circle_size;
    float object_radius = 54.4f - 4.48f * cs;
    float scaled_radius = g_scaler.scale_value( object_radius );
    float fov_radius = scaled_radius + config::fov;

    // helper to convert float[4] (0-1) to render::color (0-255)
    // clang-format off
    auto to_col = []( const float* c, float alpha_mult = 1.0f ) {
      return render::color{
        static_cast< uint8_t >( c[ 0 ] * 255.0f ),
        static_cast< uint8_t >( c[ 1 ] * 255.0f ),
        static_cast< uint8_t >( c[ 2 ] * 255.0f ),
        static_cast< uint8_t >( c[ 3 ] * alpha_mult * 255.0f )
      };
    };
    // clang-format on

    // draw FOV circle around current cursor position
    render::fill_circle( g_adjusted_pos, fov_radius, to_col( config::col_fov ) );
    render::draw_circle( g_adjusted_pos, fov_radius, to_col( config::col_fov, 1.3f ), 1.5f );

    // draw target info if we have one
    if ( obj ) {
      vec2 target = g_scaler.to_screen( obj->get_position() );

      // line from cursor to target center
      float dist = ( target - g_adjusted_pos ).length();

      if ( dist > 2.0f ) 
        render::draw_line( g_adjusted_pos, target, to_col( config::col_offset ), config::offset_thickness );

      // subtle fill on target
      render::fill_circle( target, scaled_radius, to_col( config::col_target, 0.2f ) );

      // hitcircle outline
      render::draw_circle( target, scaled_radius, to_col( config::col_target ), 2.0f );
    }

    // small crosshair at adjusted cursor position
    render::fill_circle( g_adjusted_pos, 4.0f, to_col( config::col_cursor ) );
    render::draw_circle( g_adjusted_pos, 4.0f, { 0, 0, 0, 150 }, 1.0f );

    // minimal HUD in corner
    float offset_mag = g_offset.length();

    if ( offset_mag > 0.5f ) {
      char buf[ 32 ];
      snprintf( buf, sizeof( buf ), "%.1fpx", offset_mag );

      render::draw_text( { 10, 10 }, buf, to_col( config::col_offset ) );
    }
  }

  // hooks
  using apply_input_t = void( __fastcall* )( void*, input_state*, void* );
  static apply_input_t original_apply_input = nullptr;

  using update_state_t = void( __fastcall* )( player* );
  static update_state_t original_update_gameplay_state = nullptr;

  void __fastcall hooked_update_gameplay_state( player* local ) {
    g_player = local;
    g_scaler.update();

    original_update_gameplay_state( local );
  }

  void __fastcall hooked_apply_input( void* thisptr, input_state* state, void* handler ) {
    original_apply_input( thisptr, state, handler );

    if ( !g_player || !g_player->is_playing() )
      return;

    if ( !state || !state->mouse )
      return;

    state->mouse->position = process( state->mouse->position );
  }

  constexpr auto mouse_apply_fn = "57 56 55 53 48 83 EC 48 C5 F8 77 C5 F8 29 74 24 30 C5 F8 29 7C 24 20 48 8B";
  constexpr auto update_gameplay_state_fn = "56 53 48 83 EC 28 48 8B D9 48 8B 8B 58 04 00 00 48 8B 89 10 03 00 00";

  void init() {
    void* mouse_apply = nullptr;
    void* update_gameplay = nullptr;

    while ( !mouse_apply || !update_gameplay ) {
      if ( !mouse_apply )
        mouse_apply = utils::find_pattern( mouse_apply_fn );

      if ( !update_gameplay )
        update_gameplay = utils::find_pattern( update_gameplay_state_fn );

      Sleep( 500 );
    }

    LOG( "aim: found mouse_apply @ %p", mouse_apply );
    LOG( "aim: found update_gameplay @ %p", update_gameplay );

    SETUP_HOOK( mouse_apply, hooked_apply_input, original_apply_input );
    SETUP_HOOK( update_gameplay, hooked_update_gameplay_state, original_update_gameplay_state );
  }
} // namespace features::aim
