#include "aim.hxx"
#include <utils/utils.hxx>
#include <hook/hook.hxx>

namespace features::aim {
  // TODO: define input_state struct in sdk/

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

  void init( void* mouse_apply ) {
    SETUP_HOOK( mouse_apply, hooked_apply_input, original );
    LOG( "aim: initialized" );
  }
}
