#pragma once

#include <cstdint>

struct framed_clock {
  char _pad0[ 0x30 ];
  double current_time; // +0x30
};

struct framed_beatmap_clock {
  char _pad0[ 0x210 ];
  framed_clock* final_clock_source; // +0x210
};

inline double get_clock_time( framed_beatmap_clock* clock ) {
  if ( !clock || !clock->final_clock_source )
    return 0.0;

  return clock->final_clock_source->current_time;
}