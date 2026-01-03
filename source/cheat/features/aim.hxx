#pragma once

namespace features::aim {
  namespace config {
    inline float pull = 0.014f;
    inline float decay = 0.95f;
    inline float fov = 25.0f;
    inline bool display_overlay = true;
    inline float offset_thickness = 3.0f;

    // overlay colors (rgba 0-1 for imgui)
    inline float col_fov[ 4 ] = { 0.39f, 0.70f, 1.0f, 0.24f };
    inline float col_target[ 4 ] = { 1.0f, 0.31f, 0.47f, 1.0f };
    inline float col_offset[ 4 ] = { 0.39f, 1.0f, 0.59f, 0.70f };
    inline float col_cursor[ 4 ] = { 1.0f, 0.0f, 0.0f, 0.78f };
  } // namespace config

  void init();
  void draw();
} // namespace features::aim
