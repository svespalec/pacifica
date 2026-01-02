#pragma once

#include <cmath>

struct vec2 {
  float x;
  float y;

  vec2 operator+ ( const vec2& o ) const {
    return { x + o.x, y + o.y };
  }

  vec2 operator- ( const vec2& o ) const {
    return { x - o.x, y - o.y };
  }

  vec2 operator* ( float s ) const {
    return { x * s, y * s };
  }

  vec2 operator/ ( float s ) const {
    return { x / s, y / s };
  }

  vec2& operator+= ( const vec2& o ) {
    x += o.x;
    y += o.y;

    return *this;
  }

  vec2& operator-= ( const vec2& o ) {
    x -= o.x;
    y -= o.y;

    return *this;
  }

  float length() const {
    return sqrtf( x * x + y * y );
  }

  float length_sq() const {
    return x * x + y * y;
  }

  float dot( const vec2& o ) const {
    return x * o.x + y * o.y;
  }

  vec2 normalized() const {
    float len = length();

    if ( len < 0.0001f )
      return { 0, 0 };

    return { x / len, y / len };
  }

  static vec2 zero() {
    return { 0, 0 };
  }
};
