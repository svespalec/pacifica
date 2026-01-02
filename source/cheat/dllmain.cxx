#include "pch.hxx"
#include "utils/utils.hxx"
#include "hook/hook.hxx"

static HMODULE g_module {};

DWORD WINAPI init_thread( LPVOID ) {
  utils::init_console();

  LOG( "pacifica has been loaded!" );

  if ( !hook::init() ) {
    LOG( "failed to init hooks" );
    return 1;
  }

  // TODO: main logic here

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
