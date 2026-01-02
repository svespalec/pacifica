#include "pch.hxx"
#include "utils.hxx"

void utils::init_console() {
  AllocConsole();

  FILE* fp {};

  freopen_s( &fp, "CONIN$", "r", stdin );
  freopen_s( &fp, "CONOUT$", "w", stdout );
  freopen_s( &fp, "CONOUT$", "w", stderr );

  SetConsoleTitleA( "pacifica" );
}

void utils::free_console() {
  FreeConsole();
}
