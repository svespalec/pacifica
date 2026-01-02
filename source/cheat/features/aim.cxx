#include "aim.hxx"

#include <utils/utils.hxx>
#include <hook/hook.hxx>
#include <sdk/sdk.hxx>

namespace features::aim {
  static player* g_player = nullptr;

  using apply_input_t = void( __fastcall* )( void*, input_state*, void* );
  static apply_input_t original_apply_input = nullptr;

  using update_state_t = void( __fastcall* )( player* );
  static update_state_t original_update_gameplay_state = nullptr;

  void __fastcall hooked_update_gameplay_state( player* local ) {
    g_player = local;

    original_update_gameplay_state( local );
  }

  void __fastcall hooked_apply_input( void* thisptr, input_state* state, void* handler ) {
    original_apply_input( thisptr, state, handler );

    if ( !g_player )
      return;

    if ( !g_player->is_playing() )
      return;

    // state->mouse->position = process( state->mouse->position );
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
