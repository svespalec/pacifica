#include "pch.hxx"
#include "utils.hxx"

void utils::init_console() {
  AllocConsole();

  FILE* file {};

  freopen_s( &file, "CONIN$", "r", stdin );
  freopen_s( &file, "CONOUT$", "w", stdout );
  freopen_s( &file, "CONOUT$", "w", stderr );

  SetConsoleTitleA( "pacifica v" BUILD_NUMBER );
}

void utils::free_console() {
  FreeConsole();
}
