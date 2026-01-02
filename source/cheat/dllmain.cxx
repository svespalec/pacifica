#include "pch.hxx"
#include "utils/utils.hxx"
#include "hook/hook.hxx"

static HMODULE g_module {};

DWORD WINAPI init_thread( LPVOID ) {
  utils::init_console();

  LOG( "[*] waiting for clrjit.dll...\n" );

  while ( !GetModuleHandleA( "clrjit.dll" ) ) {
    Sleep( 100 );
  }

  LOG( "pacifica loaded! build: " BUILD_NUMBER );

  if ( !hook::init() ) {
    LOG( "failed to init hooks" );
    return 1;
  }

  // TODO: main logic here
  auto addr = utils::find_pattern( "48 8B 05 ? ? ? ? 48 85 C0" );

  LOG( "addr: %p", addr );

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
