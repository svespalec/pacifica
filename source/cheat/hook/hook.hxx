#pragma once

namespace hook {
  bool init();
  void shutdown();

  bool create( void* target, void* detour, void** original );
  bool enable( void* target );
  bool disable( void* target );
} // namespace hook

#define SETUP_HOOK( target, detour, original ) \
  hook::create( reinterpret_cast< void* >( target ), reinterpret_cast< void* >( detour ), reinterpret_cast< void** >( &original ) )