#pragma once

namespace features::aim {
  namespace config {
    inline float pull = 0.014f;
    inline float decay = 0.95f;
    inline float fov = 25.0f;
    inline bool display_overlay = true;
  } // namespace config

  void init();
  void draw();
} // namespace features::aim
