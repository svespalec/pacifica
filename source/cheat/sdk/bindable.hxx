#pragma once

template < typename T >
struct bindable {
  void* method_table;
  void* value_changed;
  void* disabled_changed;
  void* default_changed;
  void* weak_reference_instance;
  void* bindings;
  void* description;
  void* leased_bindable;
  T value; // 0x40
  T default_value;
};