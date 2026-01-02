#include "pch.hxx"
#include "hook.hxx"

#include <vector>

static std::vector< std::pair< void**, void* > > g_hooks;

bool hook::init() {
  LOG( "detours initialized" );
  return true;
}

void hook::shutdown() {
  DetourTransactionBegin();
  DetourUpdateThread( GetCurrentThread() );

  for ( auto& [ original, detour ] : g_hooks )
    DetourDetach( original, detour );

  DetourTransactionCommit();

  g_hooks.clear();

  LOG( "detours shutdown" );
}

bool hook::create( void* target, void* detour, void** original ) {
  *original = target;

  DetourTransactionBegin();
  DetourUpdateThread( GetCurrentThread() );

  if ( DetourAttach( original, detour ) != NO_ERROR ) {
    DetourTransactionAbort();
    LOG( "failed to attach hook at %p", target );
    return false;
  }

  if ( DetourTransactionCommit() != NO_ERROR ) {
    LOG( "failed to commit hook at %p", target );
    return false;
  }

  g_hooks.emplace_back( original, detour );

  LOG( "hooked %p -> %p", target, detour );

  return true;
}

bool hook::enable( void* target ) {
  for ( auto& [ original, detour ] : g_hooks ) {
    if ( *original == target || detour == target ) {
      DetourTransactionBegin();
      DetourUpdateThread( GetCurrentThread() );
      DetourAttach( original, detour );

      return DetourTransactionCommit() == NO_ERROR;
    }
  }

  return false;
}

bool hook::disable( void* target ) {
  for ( auto& [ original, detour ] : g_hooks ) {
    if ( *original == target || detour == target ) {
      DetourTransactionBegin();
      DetourUpdateThread( GetCurrentThread() );
      DetourDetach( original, detour );

      return DetourTransactionCommit() == NO_ERROR;
    }
  }

  return false;
}
