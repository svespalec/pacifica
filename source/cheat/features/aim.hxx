#pragma once

namespace features::aim {
  namespace config {
    inline float pull = 0.02f;
    inline float decay = 0.97f;
    inline float fov = 15.0f;
    inline bool display_overlay = false;
  } // namespace config

  void init();
  void draw();
} // namespace features::aim
