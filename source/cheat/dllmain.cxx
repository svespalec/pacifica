#include "pch.hxx"
#include "utils/utils.hxx"
#include "hook/hook.hxx"
#include "features/aim.hxx"

static HMODULE g_module {};

DWORD WINAPI init_thread( LPVOID ) {
  utils::init_console();

  LOG( "pacifica loaded! build: " BUILD_NUMBER );
  LOG( "waiting for clrjit.dll..." );

  while ( !GetModuleHandleA( "clrjit.dll" ) ) {
    Sleep( 100 );
  }

  if ( !hook::init() ) {
    LOG( "failed to init hooks" );
    return 1;
  }

  // wait for the code to get jitted
  LOG( "waiting for patterns..." );

  void* mouse_apply = nullptr;

  while ( !mouse_apply ) {
    mouse_apply = utils::find_pattern(
      "57 56 55 53 48 83 EC 48 C5 F8 77 C5 F8 29 74 24 30 C5 F8 29 7C 24 20 "
      "48 8B D9 48 8B F2 49 8B F8 48 8B 6E ?? 80 7D ?? 00 74 ? C5 FA 10 45 ?? "
      "C5 F8 28 C8 C5 FA 10 55 ?? C5 F8 28 DA C5 FA 10 63 ?? C5 FA 10 6B ?? C5 F8 2E CC"
    );

    Sleep( 500 );
  }

  LOG( "found mouse_apply: %p", mouse_apply );

  features::aim::init( mouse_apply );

  return 0;
}

BOOL APIENTRY DllMain( HMODULE module, DWORD reason, LPVOID ) {
  if ( reason == DLL_PROCESS_ATTACH ) {
    g_module = module;

    DisableThreadLibraryCalls( module );
    CreateThread( nullptr, 0, init_thread, nullptr, 0, nullptr );
  }

  return TRUE;
}
