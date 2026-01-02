#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>

#define LOG( fmt, ... ) printf( "[ loader ] " fmt "\n", ##__VA_ARGS__ )

DWORD get_pid( const char* name ) {
  HANDLE snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

  if ( snap == INVALID_HANDLE_VALUE )
    return 0;

  PROCESSENTRY32 entry { sizeof( entry ) };
  DWORD pid = 0;

  while ( Process32Next( snap, &entry ) )
    if ( !_stricmp( entry.szExeFile, name ) )
      pid = entry.th32ProcessID;

  CloseHandle( snap );

  return pid;
}

bool inject( DWORD pid, const char* dll ) {
  HANDLE proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );

  if ( !proc )
    return false;

  size_t len = strlen( dll ) + 1;

  auto mem = VirtualAllocEx( proc, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

  bool wrote = mem && WriteProcessMemory( proc, mem, dll, len, nullptr );
  bool thread = wrote && CreateRemoteThread( proc, nullptr, 0, ( LPTHREAD_START_ROUTINE ) LoadLibraryA, mem, 0, nullptr );

  CloseHandle( proc );

  return thread;
}

int main() {
  LOG( "waiting for osu!.exe..." );

  DWORD pid = 0;

  while ( !( pid = get_pid( "osu!.exe" ) ) )
    Sleep( 100 );

  LOG( "found (pid: %lu), waiting for init...", pid );

  HANDLE proc = OpenProcess( SYNCHRONIZE, FALSE, pid );

  if ( proc ) {
    WaitForInputIdle( proc, 10000 );
    CloseHandle( proc );
  }

  char path[ MAX_PATH ];
  GetModuleFileNameA( nullptr, path, MAX_PATH );

  *strrchr( path, '\\' ) = 0;
  strcat_s( path, "\\cheat.dll" );

  LOG( "injecting..." );

  if ( inject( pid, path ) )
    LOG( "success!" );
  else
    LOG( "failed!" );

  Sleep( 1500 );
}
