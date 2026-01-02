#include "aim.hxx"
#include <utils/utils.hxx>
#include <hook/hook.hxx>

namespace features::aim {
  constexpr auto mouse_apply_pattern = "57 56 55 53 48 83 EC 48 C5 F8 77 C5 F8 29 74 24 30 C5 F8 29 7C 24 20 48 8B";

  using apply_input_t = void( __fastcall* )( void*, void*, void* );
  static apply_input_t original = nullptr;

  void __fastcall hooked_apply_input( void* thisptr, void* state, void* handler ) {
    original( thisptr, state, handler );

    // TODO: implement aim logic
    // if ( !g_state.in_gameplay )
    //   return;
    //
    // state->mouse->position = process( state->mouse->position );
  }

  void init() {
    void* mouse_apply = nullptr;

    while ( !mouse_apply ) {
      mouse_apply = utils::find_pattern( mouse_apply_pattern );
      Sleep( 500 );
    }

    LOG( "aim: found mouse_apply @ %p", mouse_apply );

    SETUP_HOOK( mouse_apply, hooked_apply_input, original );
  }
} // namespace features::aim
