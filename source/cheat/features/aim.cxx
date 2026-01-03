#include "aim.hxx"

#include <utils/utils.hxx>
#include <hook/hook.hxx>
#include <sdk/sdk.hxx>
#include <render/render.hxx>

namespace features::aim {

  // state
  static player* g_player = nullptr;
  static vec2 g_offset = {};

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
    // always decay offset so never sudden changes
    g_offset = g_offset * config::decay;

    double time = g_player->get_current_time();
    auto* obj = find_target( time );

    if ( !obj )
      return pos + g_offset;

    vec2 target = g_scaler.to_screen( obj->get_position() );
    vec2 to_target = target - ( pos + g_offset );

    float dist = to_target.length();

    float cs = g_player->state->map->difficulty->circle_size;
    float radius = 54.4f - 4.48f * cs;
    float assist_radius = g_scaler.scale_value( radius ) + config::fov;

    // in range, add tiny pull toward target
    if ( dist < assist_radius && dist > 1.0f )
      g_offset = g_offset + to_target * config::pull;

    return pos + g_offset;
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

  void draw() {
    if ( !config::display_overlay )
      return;

    if ( !g_player || !g_player->is_playing() )
      return;

    if ( !g_player->state || !g_player->state->map || !g_player->state->map->difficulty )
      return;

    // get current mouse position from last processed input
    double time = g_player->get_current_time();
    auto* obj = find_target( time );

    // calculate assist radius
    float cs = g_player->state->map->difficulty->circle_size;
    float object_radius = 54.4f - 4.48f * cs;
    float scaled_radius = g_scaler.scale_value( object_radius );
    float assist_radius = scaled_radius + config::fov;

    // we need to get the current cursor position. use the offset we're tracking
    // since we don't have direct access to mouse pos here, approximate from last frame

    if ( obj ) {
      vec2 target = g_scaler.to_screen( obj->get_position() );

      // blue circle showing assist radius around target
      render::fill_circle( target, assist_radius, { 75, 75, 255, 50 } );

      // object hitcircle outline
      render::draw_circle( target, scaled_radius, { 255, 255, 255, 100 }, 2.0f );
    }

    // green text showing current offset magnitude at screen center as reference
    float offset_mag = g_offset.length();

    if ( offset_mag > 0.5f ) {
      // draw offset indicator at top-left for debugging
      render::draw_text( { 10, 30 }, "offset:", { 255, 255, 255, 200 } );

      char buf[ 64 ];
      snprintf( buf, sizeof( buf ), "  %.1f px", offset_mag );
      render::draw_text( { 10, 50 }, buf, { 75, 255, 75, 200 } );
    }
  }

} // namespace features::aim
