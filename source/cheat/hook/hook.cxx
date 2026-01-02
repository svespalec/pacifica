#include "pch.hxx"
#include "hook.hxx"

bool hook::init() {
  if ( MH_Initialize() != MH_OK ) {
    LOG( "failed to initialize minhook" );
    return false;
  }

  LOG( "minhook initialized" );

  return true;
}

void hook::shutdown() {
  MH_DisableHook( MH_ALL_HOOKS );
  MH_Uninitialize();

  LOG( "minhook shutdown" );
}

bool hook::create( void* target, void* detour, void** original ) {
  if ( MH_CreateHook( target, detour, original ) != MH_OK ) {
    LOG( "failed to create hook at %p", target );
    return false;
  }

  if ( MH_EnableHook( target ) != MH_OK ) {
    LOG( "failed to enable hook at %p", target );
    return false;
  }

  LOG( "hooked %p -> %p", target, detour );

  return true;
}

bool hook::enable( void* target ) {
  return MH_EnableHook( target ) == MH_OK;
}

bool hook::disable( void* target ) {
  return MH_DisableHook( target ) == MH_OK;
}