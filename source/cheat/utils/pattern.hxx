#pragma once

#include <vector>
#include <cstdint>

// osu!lazer is .net core, so jitted code lives in dynamically allocated
// executable memory, not in a pe module. standard module based pattern scans
// wont work here, so we scan all committed executable regions instead.

namespace utils {
  inline auto pattern_to_bytes( const char* pattern ) {
    std::vector< int > bytes {};

    auto cur = pattern;
    auto end = pattern + strlen( pattern );

    while ( cur < end ) {
      if ( *cur == '?' ) {
        ++cur;
        if ( *cur == '?' )
          ++cur;
        bytes.push_back( -1 );
      } else {
        bytes.push_back( strtoul( cur, const_cast< char** >( &cur ), 16 ) );
      }
    }

    return bytes;
  }

  inline std::uint8_t* find_pattern( const char* pattern ) {
    auto bytes = pattern_to_bytes( pattern );
    auto size = bytes.size();
    auto data = bytes.data();

    MEMORY_BASIC_INFORMATION mbi {};
    std::uint8_t* addr = nullptr;

    while ( VirtualQuery( addr, &mbi, sizeof( mbi ) ) ) {
      // only scan committed executable memory, skip guarded pages
      // 0xf0 = PAGE_EXECUTE (0x10) | PAGE_EXECUTE_READ (0x20) | PAGE_EXECUTE_READWRITE (0x40) | PAGE_EXECUTE_WRITECOPY (0x80)
      bool executable = mbi.State == MEM_COMMIT && ( mbi.Protect & 0xf0 );

      bool guarded = mbi.Protect & PAGE_GUARD;

      if ( executable && !guarded ) {
        auto base = static_cast< std::uint8_t* >( mbi.BaseAddress );
        auto region = mbi.RegionSize;

        if ( region >= size ) {
          for ( size_t i = 0; i < region - size; ++i ) {
            bool found = true;

            for ( size_t j = 0; j < size; ++j ) {
              if ( base[ i + j ] != data[ j ] && data[ j ] != -1 ) {
                found = false;
                break;
              }
            }

            if ( found )
              return &base[ i ];
          }
        }
      }

      addr = static_cast< std::uint8_t* >( mbi.BaseAddress ) + mbi.RegionSize;
    }

    return nullptr;
  }
} // namespace utils
