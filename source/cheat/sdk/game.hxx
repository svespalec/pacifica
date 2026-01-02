#pragma once

#include "bindable.hxx"
#include "vec2.hxx"
#include "list.hxx"
#include "clock.hxx"

struct hit_object {
  char _pad0[ 0x10 ];
  bindable< double >* start_time; // +0x10
  char _pad1[ 0x40 ];
  vec2 position; // +0x58

  double get_start_time() const {
    if ( !start_time )
      return 0.0;

    return start_time->value;
  }

  vec2 get_position() const {
    return position;
  }
};

struct beatmap_difficulty {
  char _pad0[ 0x28 ];
  float drain_rate;         // 0x28
  float circle_size;        // 0x2C
  float overall_difficulty; // 0x30
  float approach_rate;      // 0x34

  // 300 (great) window: DifficultyRange(80, 50, 20)
  double get_hit_window_300() const {
    return floor( 80.0 - 6.0 * overall_difficulty ) - 0.5;
  }

  // 100 (ok) window: DifficultyRange(140, 100, 60)
  double get_hit_window_100() const {
    return floor( 140.0 - 8.0 * overall_difficulty ) - 0.5;
  }

  // 50 (meh) window: DifficultyRange(200, 150, 100)
  double get_hit_window_50() const {
    return floor( 200.0 - 10.0 * overall_difficulty ) - 0.5;
  }
};

struct beatmap {
  char _pad0[ 0x08 ];
  beatmap_difficulty* difficulty; // +0x08
  char _pad1[ 0x20 ];
  net_list* hit_objects; // +0x30
};

struct gameplay_state {
  char _pad0[ 0x08 ];
  beatmap* map; // +0x08
};

struct gameplay_clock_container {
  char _pad0[ 0x340 ];
  framed_beatmap_clock* gameplay_clock; // +0x340
};

struct player {
  char _pad0[ 0x3C8 ];
  bindable< bool >* playing; // +0x3C8
  char _pad1[ 0x38 ];
  gameplay_state* state; // +0x408
  char _pad2[ 0x58 ];
  gameplay_clock_container* clock; // +0x468

  double get_current_time() const {
    if ( !clock || !clock->gameplay_clock )
      return 0.0;

    return get_clock_time( clock->gameplay_clock );
  }

  bool is_playing() const {
    return playing && playing->value;
  }
};