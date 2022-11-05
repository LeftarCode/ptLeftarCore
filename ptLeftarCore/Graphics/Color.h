#pragma once
#include <math.h>
#include <stdint.h>

struct ImageColor {
  uint8_t r, g, b;
};

class Color {
 public:
  float r, g, b;

  Color operator*(float f) {
    Color out;
    out.r = r * f;
    out.g = g * f;
    out.b = b * f;

    return out;
  }

  Color operator+(float f) {
    Color out;
    out.r = r + f;
    out.g = g + f;
    out.b = b + f;

    return out;
  }

  Color operator-(float f) {
    Color out;
    out.r = r - f;
    out.g = g - f;
    out.b = b - f;

    return out;
  }

  Color operator/(float f) {
    Color out;
    out.r = r / f;
    out.g = g / f;
    out.b = b / f;

    return out;
  }

  Color operator+(Color c) {
    Color out;
    out.r = r + c.r;
    out.g = g + c.g;
    out.b = b + c.b;

    return out;
  }

  Color operator*(Color c) {
    Color out;
    out.r = r * c.r;
    out.g = g * c.g;
    out.b = b * c.b;

    return out;
  }

  void operator/=(float f) {
    r /= f;
    g /= f;
    b /= f;
  }

  void operator+=(Color c) {
    r += c.r;
    g += c.g;
    b += c.b;
  }

  void operator/=(Color c) {
    r /= c.r;
    g /= c.g;
    b /= c.b;
  }
};
