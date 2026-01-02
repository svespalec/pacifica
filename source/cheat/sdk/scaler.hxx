#pragma once

#include "vec2.hxx"
#include <Windows.h>

inline constexpr float g_playfield_width = 512.0f;
inline constexpr float g_playfield_height = 384.0f;

struct sdl_window {
  char _pad[ 0x28 ];
  int width;
  int height;
};

inline sdl_window* get_osu_window() {
  static HMODULE sdl = GetModuleHandleA( "SDL2.dll" );

  if ( !sdl )
    return {};

  static auto get_window = reinterpret_cast< sdl_window* ( * ) ( uint32_t ) >( GetProcAddress( sdl, "SDL_GetWindowFromID" ) );

  if ( !get_window )
    return {};

  // id = 1 (main window)
  return get_window( 1 );
}

struct playfield_scaler {
  float window_width = 1920.0f;
  float window_height = 1080.0f;

  void update() {
    auto* window = get_osu_window();

    if ( window ) {
      window_width = static_cast< float >( window->width );
      window_height = static_cast< float >( window->height );
    }
  }

  float get_scale() const {
    return window_height / 480.0f;
  }

  vec2 to_screen( vec2 pos ) const {
    float ratio = get_scale();

    float scaled_w = g_playfield_width * ratio;
    float scaled_h = g_playfield_height * ratio;

    float off_x = ( window_width - scaled_w ) * 0.5f;
    float off_y = ( ( window_height - scaled_h ) / 4.0f ) * 3.0f + ( -16.0f * ratio );

    return { scaled_w * ( pos.x / g_playfield_width ) + off_x, scaled_h * ( pos.y / g_playfield_height ) + off_y };
  }

  float scale_value( float v ) const {
    return v * get_scale();
  }
};

inline playfield_scaler g_scaler;
