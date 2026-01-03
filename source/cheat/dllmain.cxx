#include "pch.hxx"
#include "utils/utils.hxx"
#include "hook/hook.hxx"
#include "features/aim.hxx"
#include "menu/menu.hxx"

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

  // init menu first, dxgi exists before jit code
  features::menu::init();

  LOG( "waiting for patterns..." );

  features::aim::init();

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
