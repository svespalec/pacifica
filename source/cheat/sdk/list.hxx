#pragma once

#include <cstdint>

struct net_array {
  void* method_table;
  uint64_t length;
  // data starts at +0x10
};

struct net_list {
  void* method_table;
  net_array* items;
  int32_t size;
  int32_t version;
};

template < typename T >
inline T* get_list_item( net_list* list, int index ) {
  if ( !list || !list->items || index < 0 || index >= list->size )
    return nullptr;

  // net_array layout: 
  // [method_table][length][data...]
  // +0x00         +0x08   +0x10
  auto* items = reinterpret_cast< void** >( reinterpret_cast< std::uintptr_t >( list->items ) + 0x10 );

  return ( T* ) items[ index ];
}
